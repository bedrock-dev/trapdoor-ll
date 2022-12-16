if not exist %1\SDK-cpp\Lib\bedrock_server_api.lib goto process
if not exist %1\SDK-cpp\Lib\bedrock_server_var.lib goto process
goto end

:process
cd /d %1\SDK-cpp\tools\
LibraryBuilder.exe -o ..\lib\
:end
