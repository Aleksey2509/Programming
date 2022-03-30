CXX = g++
CXXFLAGS = -Wall -g -std=c++2a
CXXFLAGS += -Iheaders

HEADL = headers

SRCL = sources
SOURCES = controller.cpp view.cpp textView.cpp graphView.cpp main.cpp model.cpp
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
	$(CXX) $(CXXFLAGS) $(OBJ) -o $(PROG)

clean:
	$(RM) $(OBJ)
	$(RM) $(PROG)
	$(RM) *.dSYM

-include .depend