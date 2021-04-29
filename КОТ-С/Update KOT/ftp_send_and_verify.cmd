mkdir temp
ftp -s:ftp_cmd_send.txt
pause
ftp -s:ftp_cmd_get.txt
fc /N *.INI temp\*.ini
rmdir /s /q temp
pause