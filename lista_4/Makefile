default: zadanie4

%: %.cpp
	g++ -I. $< -o $@ AGL3Window.cpp shader.cpp -lepoxy -lGL -lglfw -g
clean:
	rm a.out *.o *~ zadanie4
