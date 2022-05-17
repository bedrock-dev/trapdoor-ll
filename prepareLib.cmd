if not exist %1\SDK\Lib\bedrock_server_api.lib goto process
if not exist %1\SDK\Lib\bedrock_server_var.lib goto process
goto end

:process
cd /d %1\SDK\Tools\
LibraryBuilder.exe -o ..\Lib\
:end
