CXX = g++
CXXFLAGS = -Wall -g -std=c++2a
CXXFLAGS += -Iheaders -I/usr/local/Cellar/sfml/2.5.1_1/include

LDFLAGS += -lsfml-graphics -lsfml-window -lsfml-system 

HEADL = headers

SRCL = sources
SOURCES = controller.cpp view.cpp textView.cpp graphView.cpp main.cpp model.cpp adapter.cpp
SRC := $(addprefix $(SRCL)/, $(SOURCES))

OBJL = object
OBJECTS = $(SOURCES:.cpp=.o)
OBJ := $(addprefix $(OBJL)/, $(OBJECTS))

PROG = snake
RM = rm -f -r

$(OBJL)/%.o: $(SRCL)/%.cpp
	$(CXX) $(CXXFLAGS) -c -o $@ $<

all: $(PROG)

depend:
	$(CXX) $(CXXFLAGS) $(SRC) -MM > .depend

$(PROG): $(OBJ)
	$(CXX) $(CXXFLAGS) $(OBJ) -o $(PROG) $(LDFLAGS)

sfmlWindow: sfmlTest.cpp
	$(CXX) -c sfmlTest.cpp $(CXXFLAGS) -o sfmlTest.o
	$(CXX) sfmlTest.o -o sfml-app $(LDFLAGS)

clean:
	$(RM) $(OBJ)
	$(RM) $(PROG)
	$(RM) *.dSYM

-include .depend