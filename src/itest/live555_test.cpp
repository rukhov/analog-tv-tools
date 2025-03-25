/*
 * This file is part of the analot-tv-tools project.
 * Copyright (c) 2025 Roman Ukhov.
 *
 * https://github.com/rukhov/analog-tv-tools
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, version 3.
 *
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
 * General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program. If not, see <http://www.gnu.org/licenses/>.
 */

#include <gtest/gtest.h>

#include <BasicUsageEnvironment.hh>
#include <GroupsockHelper.hh>
#include <liveMedia.hh>

// Custom FramedSource to read from an internal buffer
class InternalBufferSource : public FramedSource
{
public:
    static InternalBufferSource* createNew(UsageEnvironment& env)
    {
        return new InternalBufferSource(env);
    }

protected:
    InternalBufferSource(UsageEnvironment& env) : FramedSource(env) {}

    virtual ~InternalBufferSource() {}

private:
    virtual void doGetNextFrame()
    {
        std::cout << "doGetNextFrame\n";
        // Simulate reading from an internal buffer
        static const char* testData = "This is a test frame.";
        unsigned frameSize = strlen(testData);

        if (frameSize > fMaxSize) {
            frameSize = fMaxSize;
        }

        memcpy(fTo, testData, frameSize);
        fFrameSize = frameSize;

        gettimeofday(&fPresentationTime, NULL);
        FramedSource::afterGetting(this);
    }
};

sockaddr_storage in_addr_2_sockaddr_storage(const in_addr& ipv4_addr)
{

    sockaddr_storage storage{};

    // Clear the storage structure
    memset(&storage, 0, sizeof(storage));

    // Cast to sockaddr_in for IPv4
    auto* addr_in = reinterpret_cast<sockaddr_in*>(&storage);

    // Set the address family to IPv4
    addr_in->sin_family = AF_INET;

    // Copy the IPv4 address
    addr_in->sin_addr = ipv4_addr;

    return storage;
}

void afterPlayingFunc(void* clientData) { std::cout << "afterPlayingFunc" << std::endl; }
void StartStreamTask(void* clientData) { std::cout << "StartStreamTask" << std::endl; }

void std_function_task_wrapper(void* clientData)
{
    auto func = reinterpret_cast<std::function<void()>*>(clientData);
    func->operator()();
    delete func;
}

void shedule_task(TaskScheduler& scheduler, std::function<void()> const& f)
{
    scheduler.scheduleDelayedTask(
        100000, &std_function_task_wrapper, new std::function<void()>(f));
}

/*
Key Points
You cannot directly convert InternalBufferSource* to RTCPInstance* because they are
unrelated types.

Instead, you associate an RTCPInstance with an RTP stream that uses your
InternalBufferSource as the media source.

The RTCPInstance is created separately and linked to the RTP sink.

Output
When you run this program, it will:

Create an RTP stream using your InternalBufferSource.

Set up RTCP for the RTP stream.

Start streaming data over the network.

You can use a tool like Wireshark or VLC to verify that the RTP and RTCP streams are
working correctly.

Let me know if you need further clarification!

*/
TEST(live555, server)
{
    // Create a task scheduler and environment
    TaskScheduler* scheduler = BasicTaskScheduler::createNew();
    UsageEnvironment* env = BasicUsageEnvironment::createNew(*scheduler);

    // Create groupsocks for RTP and RTCP
    struct in_addr destinationAddress;
    destinationAddress.s_addr = chooseRandomIPv4SSMAddress(*env);
    const unsigned short rtpPortNum = 18888;
    const unsigned short rtcpPortNum = rtpPortNum + 1;
    const unsigned char ttl = 255;

    Groupsock rtpGroupsock(
        *env, in_addr_2_sockaddr_storage(destinationAddress), rtpPortNum, ttl);
    Groupsock rtcpGroupsock(
        *env, in_addr_2_sockaddr_storage(destinationAddress), rtcpPortNum, ttl);

    auto start_streaming = [&]() {
        // Create a custom FramedSource
        InternalBufferSource* source = InternalBufferSource::createNew(*env);

        // Create an RTP sink (e.g., for H.264 video)
        RTPSink* sink = H264VideoRTPSink::createNew(*env, &rtpGroupsock, 96);

        // Create an RTCP instance
        RTCPInstance* rtcp = RTCPInstance::createNew(
            *env, &rtcpGroupsock, 50000, (const uint8_t*)"example@test.com", sink, NULL);
        // Start the RTCP instance
        rtcp->setByeHandler(NULL, NULL); // Optional: Set a handler for BYE packets

        // Start playing the sink
        auto res = sink->startPlaying(*source, &afterPlayingFunc, NULL);

        // Create a server media session
        ServerMediaSession* sms = ServerMediaSession::createNew(
            *env, "testStream", "Test Stream", "Session Description");
        sms->addSubsession(PassiveServerMediaSubsession::createNew(*sink, rtcp));
        RTSPServer* rtspServer = RTSPServer::createNew(*env, 8554);
        if (rtspServer == NULL) {
            *env << "Failed to create RTSP server: " << env->getResultMsg() << "\n";
            exit(1);
        }

        rtspServer->addServerMediaSession(sms);

        // Announce the stream
        char* url = rtspServer->rtspURL(sms);
        *env << "Stream is available at: " << url << "\n";
        delete[] url;
    };

    shedule_task(*scheduler, start_streaming);

    std::printf("Starting event loop.\n");
    // Start the event loop
    scheduler->doEventLoop();

    EXPECT_TRUE(true);
}