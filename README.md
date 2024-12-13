## CGDoom pour IN330

CGDoom est la 5ème génération d'un port source de DOOM. C'est essentiellement le [DOOM officiel](https://github.com/id-Software/DOOM) mais modifié plusieurs fois d'affilée pour enlever des dépendances système. Il y a des dizaines de ports source de DOOM dans la nature, et on pourrait en choisir un autre, mais j'ai (Sébastien Michelland) travaillé sur CGDoom ce qui simplifie la construction du TP.

## Obtenir la bonne toolchain

Lorsqu'on va compiler CGDoom pour RISC-V on va combiner du code venant de 3 sources :

1. Le code de CGDoom, qui vient de ce dépôt ;
2. Du code de la bibliothèque standard newlib, notamment `sprintf()` qui est utilisée par DOOM ;
3. Du code venant de `libgcc`, une bibliothèque dans laquelle GCC implémente les opérations non supportées par le matériel (genre les divisions si vous les avez pas dans votre émulateur).

Or, ces trois bouts de code, pour coexister, ont *absolument* besoin de se mettre d'accord sur deux choses :

- **L'architecture**, c'est-à-dire RISC-V 64 mais aussi exactement quelles extensions. Ici on compile pour le plus petit ensemble d'instructions possible pour RISC-V 64, qui ne contient que le jeu d'instructions de base "`I`", et est donc noté `rv64i`.
- **L'ABI** (Application Binary Interface), qui désigne les conventions établies pour que le code binaire puisse bien s'entendre avec lui-même. L'ABI indique notamment quels types de données primitifs existent, leur taille (notamment celle des `long` et des pointeurs), et les conventions d'appel. On utilise l'ABI `lp64` qui est canonique pour le RISC-V 64 sans support du calcul en point flottant.

Comme `rv64i` est très minimal, il n'est pas totalement trivial de trouver un compilateur et une bibliothèque standard compilées avec cette option. Il est tout à fait possible que le compilateur que vous avez utilisé jusqu'à présent dans le projet ne fournisse libgcc que pour `rv64gc`, un jeu d'instructions sensiblement plus gros. Même si le compilo accepte de compiler avec `-march=rv64i -mabi=lp64`, s'il n'a pas la bonne libgcc pour aller, l'édition des liens échouera. C'est fourbe...



Toolchain

sudo apt install gcc-riscv64-unknown-elf

 riscv64-elf-ubuntu-22.04-gcc-nightly-2024.11.22-nightly.tar.xz
