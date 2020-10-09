default: tutorial02

%: %.cpp
	g++ -I. $< -o $@ common/shader.cpp -lGLEW  -lGL -lglfw 
clean:
	rm a.out *.o *~ tutorial02
