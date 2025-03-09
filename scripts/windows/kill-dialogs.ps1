Write-Output "Killing all the visual c++ runtime dialogs"

$Title="Microsoft Visual C++ Runtime Library"

while ($true) {
    Start-Sleep -Seconds 2
	Get-Process | Where-Object { $_.MainWindowTitle -eq $Title } | ForEach-Object { $_.Kill() }
}

