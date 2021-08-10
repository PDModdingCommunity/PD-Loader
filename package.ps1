param (
	[Parameter()]
	[ValidateSet('Debug', 'Release')]
	[string]
	$Configuration = 'Release',
	[Parameter()]
	[Switch]
	$Clean = $true
)

# Set-PSDebug -Trace 1
Push-Location $PWD

$RepoDir = $PSScriptRoot
$SolutionDir = "$RepoDir\source-code"
$OutputDir = "$SolutionDir\x64\$Configuration"
$PackageDir = "$env:TMP\full"
$PackageName = "$RepoDir\PD-Loader-$Configuration-AppVeyor.zip"

if ($Clean) {
	rm $PackageDir -Recurse
	rm $PackageName
}

md $PackageDir -Force
copy $SolutionDir\data\* $PackageDir\ -Recurse
copy $RepoDir\dnsapi.dll $PackageDir\

copy $OutputDir\*.dva $PackageDir\plugins\
copy $OutputDir\zh-Hans $PackageDir\plugins\ -Recurse

cd $PackageDir
7z a $PackageName *

Pop-Location