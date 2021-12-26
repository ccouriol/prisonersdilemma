# Prisoner's Dilemma

## Build requirements

```bash
apt install build-essential doxygen graphviz libgtk-3-dev
```

## Build & run

```bash
git clone https://github.com/ccouriol/prisonersdilemma.git
cd prisonersdilemma/

This program requires at least one running server and two running clients for a test run.

### To compile all the program :
make all

### To compile the client's part :
make client

### To compile the server's part :
make server

### To compile the program and run the server :
make all && make runserver

### To compile the program and run the client :
make all && make runclient

### To compile and build the documentation
make doc
./docs/html/index.html

### To clean the docs and program
make cleanall
```

## Mockup

[Excalidraw mockup](https://excalidraw.com/#json=s0DXZXyjgY25yq5YDQ9gv,wePrsjqY_j_UPpj-NZa47A)

![excalidraw preview](./template/Untitled-2021-11-15-1958.png)

## Dev requirements

```bash
apt install clang clang-tidy clang-format python3 python3-pip cppcheck valgrind libconfig-dev
pip3 install pre-commit
PATH="$PATH:$HOME/.local/bin"
pre-commit install --hook-type commit-msg
pre-commit install
pre-commit autoupdate
pre-commit run
```

1. [C/C++ IntelliSense, debugging, and code browsing.](https://marketplace.visualstudio.com/items?itemName=ms-vscode.cpptools)
2. [Use Clang-Format in Visual Studio Code](https://marketplace.visualstudio.com/items?itemName=xaver.clang-format)
3. [Integrates clang-tidy into VS Code](https://marketplace.visualstudio.com/items?itemName=notskm.clang-tidy)
4. [Extension for generating the doxygen documentation](https://marketplace.visualstudio.com/items?itemName=dusartvict.doc-doxygen)
5. [Improve your code commenting by annotating with alert, informational, TODOs, and more!](https://marketplace.visualstudio.com/items?itemName=aaron-bond.better-comments)
6. [commitizen - git commit with conventions](https://marketplace.visualstudio.com/items?itemName=KnisterPeter.vscode-commitizen)
7. [Improve highlighting of errors, warnings and other language diagnostics.](https://marketplace.visualstudio.com/items?itemName=usernamehw.errorlens)

```json
  "editor.formatOnSave": true,
  "errorLens.fontSize": "12",
  "errorLens.onSave": true,
  "errorLens.enabledDiagnosticLevels": ["error", "warning", "hint"],
  "workbench.colorCustomizations": {
    "errorLens.hintForeground": "#ffc600A1",
    "errorLens.hintBackground": "#ffc60003",
    "errorLens.errorBackground": "#ff000005"
  },
  "errorLens.exclude": ["never (read|used)"],
  "clang-tidy.executable": "clang-tidy",
  "files.eol": "\n",
  "editor.tabSize": 2,
  "security.workspace.trust.banner": "never",
  "security.workspace.trust.enabled": false,
  "terminal.integrated.tabs.enabled": true,
  "workbench.editor.wrapTabs": true,
```

## Dev hints

1. `ctrl shift p` > generate comments
2. Fill author, brief and remarks (optionnal)
3. `cppcheck --error-exitcode=1 --enable=all --suppress=unmatchedSuppression . 2> cppcheck.log`
4. `sudo valgrind --tool=memcheck --leak-check=summary --leak-resolution=high --show-reachable=yes ./output/main 2> valgrind.log`
5. commitizen commits, and stage clang format modification
6. `git push`
7. `git fetch --all`
8. `git pull --all` (or checkout sur main git pull git merge la branche)

## Prisoner dilemna context

> The prisoner's dilemma is a standard example of a game analyzed in game theory that shows why two completely rational individuals might not cooperate, even if it appears that it is in their best interests to do so. It was originally framed by Merrill Flood and Melvin Dresher while working at RAND in 1950. Albert W. Tucker formalized the game with prison sentence rewards and named it "prisoner's dilemma",[1] presenting it as follows:

[Wikipedia](https://www.wikiwand.com/en/Prisoner%27s_dilemma)

## Todo

- [ ] Server config example
- [ ] Client config example
- [x] Explain prisoner's dilmena
- [ ] Explain context
- [x] Build depedencies for config files
- [x] implement valgrind
- [x] change makefile to build a server executable and client executable
- [ ] gui rounds
- [x] link bet label text with button
- [ ] ? loader while game is searching

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
