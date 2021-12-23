# Prisoner's Dilemma

## Build requirements

```bash
apt install build-essential doxygen graphviz libgtk-3-dev
```

## Build & run

```bash
git clone https://github.com/ccouriol/prisonersdilemma.git
cd prisonersdilemma/

### To compile and run the client :
make client
make runclient

### To compile and run the server :
make server
make runserver

### To clean the client
make cleanclient

### To clean the server
make cleanserver

### To compile and build the documentation
make doc
./docs/html/index.html

### To clean the docs
make clean
```

## Mockup

[Excalidraw mockup](https://excalidraw.com/#json=s0DXZXyjgY25yq5YDQ9gv,wePrsjqY_j_UPpj-NZa47A)

![excalidraw preview](./template/Untitled-2021-11-15-1958.png)

## Prisoner dilemna context

> The prisoner's dilemma is a standard example of a game analyzed in game theory that shows why two completely rational individuals might not cooperate, even if it appears that it is in their best interests to do so. It was originally framed by Merrill Flood and Melvin Dresher while working at RAND in 1950. Albert W. Tucker formalized the game with prison sentence rewards and named it "prisoner's dilemma",[1] presenting it as follows:

[Wikipedia](https://www.wikiwand.com/en/Prisoner%27s_dilemma)

## Subject

### Tips

- Respecting the conditions of contract
- Manage your time well
- Have a good discussion in your team
- Think before you program
- Structure your source code to make it as scalable as possible
- Use assertions
- Formalize the protocol well (do simulations between you)
- Think about the possible evolutions of the project: for example the state of the server and the clients could be supervised from a supervision station

### Oral

4 Pages + 2 Annexes max

- The organization of the work in the group
- The methodology used in the development
- The justification of certain choices and the evaluation of the relevance afterwards
- The current status of the project and what remains to be done
- The difficulties encountered
- A quick assessment of what this project has brought you
