all:
	g++ window.cpp progression.cpp scale.cpp chord.cpp play.cpp draw.cpp resolve.cpp -ld2d1 -ldwrite -lwinmm -o harmony.exe
