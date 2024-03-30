make:
	g++ main.cpp -o mix -lduktape
	g++ modules/fetch.cpp -o ~/.mix/modules/fetch.so -shared -lduktape -lcurl -fPIC
	g++ modules/file.cpp -o ~/.mix/modules/file.so -shared -lduktape -lcurl -fPIC
	g++ modules/example.cpp -o ~/.mix/modules/example.so -shared -lduktape -fPIC
	./mix