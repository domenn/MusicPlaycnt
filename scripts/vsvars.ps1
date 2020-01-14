function EnsureVSSetupInstance {
    try {
        $_ = Get-VSSetupInstance
    } 
    catch {
        $_=Install-PackageProvider -Name NuGet -MinimumVersion 2.8.5.201 -scope CurrentUser -Force
        $_=Install-Module VSSetup -Scope CurrentUser -Confirm:$false -Force
    }
}

EnsureVSSetupInstance
$vspath=$(Get-VSSetupInstance -All | Select-VSSetupInstance -Product "*" -RequireAny -Require 'Microsoft.VisualStudio.Workload.NativeDesktop','Microsoft.VisualStudio.Component.VC.CoreBuildTools' -Latest).InstallationPath
$bitness=[IntPtr]::size * 8
$vspath += "\VC\Auxiliary\Build\vcvars" + $bitness + ".bat"

Write-Output $vspath