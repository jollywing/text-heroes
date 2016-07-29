objs = TextParser.o CFight.o CGame.o global.o ScriptEngine.o
crpg: $(objs)
	g++ -o crpg $(objs)

TextParser.o: TextParser.cpp TextParser.h global.h
	g++ -g -c TextParser.cpp
ScriptEngine.o: ScriptEngine.cpp ScriptEngine.h
	g++ -g -c ScriptEngine.cpp
CFight.o: CFight.cpp CFight.h global.h
	g++ -g -c CFight.cpp
CGame.o: CGame.cpp CGame.h TextParser.h global.h
	g++ -g -c CGame.cpp
global.o: global.cpp global.h
	g++ -g -c global.cpp

.PHONY: clean

clean:
	rm *.o
	rm crpg
