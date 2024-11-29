all: memdumper readaddress stacktest wruffhook

memdumper:
	g++ -o tools/binaries/app_memdumper tools/memdumper.cpp

readaddress:
	g++ -o tools/binaries/app_readaddress tools/readaddress.cpp

stacktest:
	g++ -o tools/binaries/app_stacktest tools/stacktest.cpp

wruffhook:
	g++ -o binaries/wruffhook main.cpp 
