Vagrant.configure("2") do |config|
  config.vm.box = "gusztavvargadr/windows-server-2022-standard"

  config.vm.communicator = "winrm"
  config.winrm.username = "vagrant"
  config.winrm.password = "vagrant"
  config.winrm.transport = :plaintext
  config.winrm.basic_auth_only = true

  config.vm.provider "hyperv" do |h|
    h.memory = 4096
    h.cpus = 2
    h.vmname = "windows-hyperv-vagrant"
    h.enable_virtualization_extensions = true
  end

  # Provisioning script to install Chocolatey and Visual Studio Build Tools
  config.vm.provision "shell", inline: <<-POWERSHELL
    # Allow script execution
    Set-ExecutionPolicy Bypass -Scope Process -Force

    # Install Chocolatey if not installed
    if (-not (Get-Command choco -ErrorAction SilentlyContinue)) {
      Write-Host "Installing Chocolatey..."
      Set-ExecutionPolicy Bypass -Scope Process -Force
      [System.Net.ServicePointManager]::SecurityProtocol = [System.Net.ServicePointManager]::SecurityProtocol -bor 3072
      iex ((New-Object System.Net.WebClient).DownloadString('https://community.chocolatey.org/install.ps1'))
    } else {
      Write-Host "Chocolatey already installed"
    }

    # Ensure Temp folder exists
    if (-not (Test-Path "C:\\Temp")) {
      New-Item -ItemType Directory -Path "C:\\Temp"
    }

    # Ensure py launcher exists
    if (-not (Get-Command py -ErrorAction SilentlyContinue)) {
        Write-Error "Python launcher (py.exe) not found"
        exit 1
    }

    # Create shim directory
    $shimDir = "C:\\Tools\\shims"
    if (-not (Test-Path $shimDir)) {
        New-Item -ItemType Directory -Path $shimDir | Out-Null
    }

    # Create python.cmd shim that forwards to py
    $pythonShim = "$shimDir\\python.cmd"
    @"
    @echo off
    py %*
    "@ | Set-Content -Encoding ASCII $pythonShim

    # Add shim directory to SYSTEM PATH if missing
    $systemPath = [Environment]::GetEnvironmentVariable("Path", "Machine")
    if ($systemPath -notlike "*$shimDir*") {
        [Environment]::SetEnvironmentVariable(
            "Path",
            "$systemPath;$shimDir",
            "Machine"
        )
    }

    Write-Host "python shim created successfully"


    # Download Visual Studio Build Tools if not already downloaded
    $vsInstallerPath = "C:\\Temp\\vs_buildtools.exe"
    if (-not (Test-Path $vsInstallerPath)) {
      Invoke-WebRequest -Uri "https://aka.ms/vs/17/release/vs_BuildTools.exe" -OutFile $vsInstallerPath
    }

    Start-Process -FilePath $vsInstallerPath -ArgumentList `
        '--quiet', '--wait', '--norestart', '--nocache', `
        '--add Microsoft.VisualStudio.Workload.VCTools', `
        '--add Microsoft.VisualStudio.Component.Windows10SDK.19041', `
        '--add Microsoft.VisualStudio.Component.VC.CMake.Project', `
        '--includeRecommended' `
        -Wait -NoNewWindow
  POWERSHELL
end
