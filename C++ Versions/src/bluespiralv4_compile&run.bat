@echo off
:loop
cls
del bluespiralv4.exe
g++ -o bluespiralv4.exe position.cc bluespiralv4.cpp
bluespiralv4.exe
pause
goto loop