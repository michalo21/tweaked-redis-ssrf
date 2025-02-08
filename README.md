# INFO
It's forked, tweaked version of great tool created by xmsec to user gopher to gain rce on Redis instances!

# Changelog
1. Added modified exp.c file to properly compile exp.so library using: https://github.com/n0b0dyCN/redis-rogue-server -> RedisModulesSDK. The modified version has reimplemented RevShellCommand function, to fork the main process and works on it to gain revershell. It prevents to end the main process (Redis) while closing revshell, so it is more gentle with your client's infrastructure while pentesting.
2. Expanding ssrf-redis.py functionality to use the RevShellCommand from exp.so.
3. rogue-server.py implemented in python3.

# Exploiting

1. Build exp.so library with my exp.c file using https://github.com/n0b0dyCN/redis-rogue-server -> RedisModulesSDK dependencies. Instruction is in the https://github.com/n0b0dyCN/redis-rogue-server repo. While building ```make clean``` for all before ```make```.
2. Place the exp.so library in the main folder with ssrf-redis.py and roughe-server.py files.
3. Modify the ssrf-redis.py file according the functioniality it provides to generate payload. If you want ReverShell -> use mode 5 and enter your lhost/lport/revhost/revport.
4. Run ```python3 ssrf-redis.py```
5. Run ```python3 rogue-server.py```
6. Don't forget abount ```nc -lvnp <port>``` if revshell.
7. If using revshell option, double send the gopher payload! If other options does not seems work, double send the payload!

## Kudos
https://github.com/xmsec/redis-ssrf
https://github.com/n0b0dyCN/redis-rogue-server
ChatGPT