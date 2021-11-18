# Prisoner's Dilemma

## Build requirements

```bash
apt install build-essential doxygen graphviz libgtk-3-dev
```

## Build & run

```bash
git clone https://github.com/ccouriol/prisonersdilemma.git
cd prisonersdilemma/

### To compile and run the program :
make run

### To compile the program and run manually :
make && ./output/main

### To compile and build the documentation
make doc
./docs/html/index.html

### To clean the docs and program
make clean
```

## Mockup

[Excalidraw mockup](https://excalidraw.com/#json=s0DXZXyjgY25yq5YDQ9gv,wePrsjqY_j_UPpj-NZa47A)

![excalidraw preview](./template/Untitled-2021-11-15-1958.png)

## Dev requirements

```bash
apt install clang clang-tidy clang-format python3 python3-pip
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

## Todo

- [ ] Server config
- [ ] Client config
- [ ] Explain prisoner's dilmena
- [ ] Explain context
- [ ] Build depedencies for config files
