function SimpleReadHost(){
    param($Prompt)
    Write-Host $Prompt -NoNewline
    $Host.UI.ReadLine()
}

$build_dir="./release-build"
#清空之前的编译缓存

if (Test-Path $build_dir){
    #Get-ChildItem -Path $build_dir -Include * | remove-Item -recurse
}
#编译
cmake -B   $build_dir -DDEV=OFF  -DCMAKE_CXX_FLAGS="/MP" ..
cmake  --build $build_dir --config Release

#获取版本号
$dll_file_name=$(Get-ChildItem -Path "$build_dir/Release" -Filter *.dll).Name
$version=$dll_file_name.Replace(".dll","")
#删除旧的目录
if(Test-Path "$version"){
    Remove-Item -LiteralPath "$version" -Force -Recurse
}

#创建目录
mkdir $version | Out-Null
mkdir "$version/plugins/" | Out-Null
mkdir "$version/plugins/trapdoor/" | Out-Null


#拷贝核心文件
Copy-Item "$build_dir/Release/*.dll" "$version/plugins"
Copy-Item "$build_dir/Release/*.pdb" "$version/plugins"

#拷贝配置文件和语言文件
$cfg_file_path = "../src/base"
Copy-Item "$cfg_file_path/*.json" "$version/plugins/trapdoor"

#拷贝其他文件
$other_files_path = ".."
Copy-Item "installer.exe" "$version"
Copy-Item "$other_files_path/README*.md" "$version"
Copy-Item "$other_files_path/LICENSE" "$version"


#生产元数据文件
$sdk_version=SimpleReadHost -Prompt "input minimum version number(2.x.x): "
$metadata="$version".Replace("trapdoor-","").Split("-") | Out-String
"$metadata$sdk_version" | Out-File -FilePath "$version/metadata"
#生成压缩文件
Compress-Archive -Path "$version" -DestinationPath "$version.zip" -Force
#删除临时文件
#Remove-Item -LiteralPath "$version" -Force -Recurse

