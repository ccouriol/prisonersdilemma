# Development docs

## Dev requirements

### Package requirements

```bash
apt install clang clang-tidy clang-format python3 python3-pip cppcheck valgrind libconfig-dev
pip3 install pre-commit
export PATH="$PATH:$HOME/.local/bin"
pre-commit install --hook-type commit-msg
pre-commit install
pre-commit autoupdate
pre-commit run
```

### VSCode extensions

1. [C/C++ IntelliSense, debugging, and code browsing.](https://marketplace.visualstudio.com/items?itemName=ms-vscode.cpptools)
2. [Use Clang-Format in Visual Studio Code](https://marketplace.visualstudio.com/items?itemName=xaver.clang-format)
3. [Integrates clang-tidy into VS Code](https://marketplace.visualstudio.com/items?itemName=notskm.clang-tidy)
4. [Extension for generating the doxygen documentation](https://marketplace.visualstudio.com/items?itemName=dusartvict.doc-doxygen)
5. [Improve your code commenting by annotating with alert, informational, TODOs, and more!](https://marketplace.visualstudio.com/items?itemName=aaron-bond.better-comments)
6. [commitizen - git commit with conventions](https://marketplace.visualstudio.com/items?itemName=KnisterPeter.vscode-commitizen)
7. [Improve highlighting of errors, warnings and other language diagnostics.](https://marketplace.visualstudio.com/items?itemName=usernamehw.errorlens)

### VSCode settings.json

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
  "clang-format.fallbackStyle": "llvm",
  "clang-tidy.executable": "/usr/bin/clang-tidy",
  "editor.defaultFormatter": "xaver.clang-format",
  "clang-tidy.compilerArgs": [
    "`pkg-config --cflags gtk+-3.0`",
    "`pkg-config --libs gtk+-3.0` -rdynamic"
  ],
  "security.workspace.trust.banner": "never",
  "security.workspace.trust.enabled": false,
  "terminal.integrated.tabs.enabled": true,
  "workbench.editor.wrapTabs": true,
```

### Before commit

1. On VSCode `ctrl shift p` > generate comments
1. Fill author, brief and remarks if necessary
1. commitizen commits, and stage clang format modification due to pre-commit
1. `git push`

### Side check

1. `sudo valgrind --tool=memcheck --leak-check=summary --leak-resolution=high --show-reachable=yes ./output/main 2> valgrind.log`
1. `cppcheck --error-exitcode=1 --enable=all --suppress=unmatchedSuppression . 2> cppcheck.log`
