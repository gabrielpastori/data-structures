A dataframe implemented in cpp that can be used in Python.<br/>
It uses a B+ tree to index columns enabling faster queries<br/>
Compiling: <br/>
Install Boost [boost](https://github.com/EMAp-EDA-2022/monitorias/blob/main/conteudo/monitoria-boost.md). <br/>
C++17 or higher.
```bash
g++ -std=c++17 -Wall -Wextra -fPIC -Wno-undef -shared -I/usr/lib/python3.8/ -I/usr/include/python3.8/ main.cpp -o capivara.so -lboost_python38 -lboost_numpy38
```

