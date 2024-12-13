## CGDoom pour IN330

CGDoom est la 5ème génération d'un port source de DOOM qui notamment été utilisé sur tout un tas de calculatrices graphiques. C'est essentiellement le [DOOM officiel](https://github.com/id-Software/DOOM) mais modifié plusieurs fois d'affilée pour enlever des dépendances système. Il y a des dizaines de ports source de DOOM dans la nature, dont certains faits spécifiquement pour être faciles à porter comme [doomgeneric](https://github.com/ozkl/doomgeneric), qui seraient utilisables aussi. Pour ce TP j'ai (Sébastien Michelland) simplement utilisé un port sur lequel j'ai travaillé dans le passé.

## Obtenir la bonne toolchain

Lorsqu'on va compiler CGDoom pour RISC-V on va combiner du code venant de 3 sources :

1. Le code de CGDoom, qui vient de ce dépôt ;
2. Du code de la bibliothèque standard newlib, notamment `sprintf()` qui est utilisée par DOOM ;
3. Du code venant de `libgcc`, une bibliothèque dans laquelle GCC implémente les opérations non supportées par le matériel (genre les divisions si vous les avez pas dans votre émulateur).

Or, ces trois bouts de code, pour coexister, ont *absolument* besoin de se mettre d'accord sur deux choses :

- **L'architecture**, c'est-à-dire RISC-V 64 mais aussi exactement quelles extensions. Ici on compile pour le plus petit ensemble d'instructions possible pour RISC-V 64, qui ne contient que le jeu d'instructions de base "`I`", et est donc noté `rv64i`.
- **L'ABI** (Application Binary Interface), qui désigne les conventions établies pour que le code binaire puisse bien s'entendre avec lui-même. L'ABI indique notamment quels types de données primitifs existent, leur taille (notamment celle des `long` et des pointeurs), et les conventions d'appel. On utilise l'ABI `lp64` qui est canonique pour le RISC-V 64 sans support du calcul en point flottant.

Il nous faut donc un compilateur qui sait compiler pour `rv64i`/`lp64` mais *aussi* qui a une version compilée de libgcc pour cette cible, et bien sûr une bibliothèque standard newlib compilée pour cette cible. Si vous prenez un "GCC pour RISC-V 64" aléatoirement dans le gestionnaire de paquets de votre distribution ou même [aux endroits habituels sur Internet](https://github.com/riscv-collab/riscv-gnu-toolchain) il est peu probable (lire : très improbable) qu'il réunisse toutes ces conditions.

Classiquement ce qui se passe c'est que comme `rv64i` est très minimal, ce n'est jamais la cible par défaut ; la cible par défaut c'est `rv64gc` qui contient beaucoup plus d'instructions. Grâce à un mécanisme appelé "multilib", un même compilateur peut fournir les bibliothèques pour plusieurs combinaisons d'architecture/ABI, mais généralement quand vous installez votre compilo off-the-shelf soit multilib est désactivé soit y'a pas `rv64i` dedans.

Vous pouvez voir la liste des configuratins multilib supportées en tapant `riscv...-gcc --print-multi-lib`, et chercher si `--enable-multilib` ou `--disable-multilib` a été spécifié lorsque le compilateur a été construit en affichant sa commande de configuration dans `riscv...-gcc -v`.

Ici, pour s'éviter les embrouilles d'avoir plein de versions différentes venant d'Internet, on va construire à la main le compilo qui nous va bien et la newlib qui nous va bien.

Consultez les dépendances nécessaires pour la construction du GCC dans [les notes du dépôt de la toolchain](https://github.com/riscv-collab/riscv-gnu-toolchain?tab=readme-ov-file#prerequisites) ; je reproduis ici pour Debian/Ubuntu/WSL.

```bash
% sudo apt-get install autoconf automake autotools-dev curl python3 python3-pip libmpc-dev libmpfr-dev libgmp-dev gawk build-essential bison flex texinfo gperf libtool patchutils bc zlib1g-dev libexpat-dev ninja-build git cmake libglib2.0-dev libslirp-dev
```

Ensuite, clônez le dépôt et configurez avec la cible qui nous intéresse. Compilez en y mettant-y du coeur si vous voulez que ça aille vite (`-j4`, `-j8`, voire plus si vous avez plus). Comptez entre 5-10 minutes sur une bonne machine et une heure sur une machine ancienne...

```bash
% git clone --depth=1 https://github.com/riscv-collab/riscv-gnu-toolchain
% ./configure --prefix=$(realpath ../riscv) --with-arch=rv64i --with-abi=lp64
% make -j4
```

Exportez votre PATH (de préférence de façon permanente), ensuite vous pouvez tester sur un petit programme.

```bash
# ajoutez dans .bashrc en développant le realpath
% export PATH="$(realpath ../riscv/bin):$PATH"

% echo -e '#include <stdio.h>\nint main(void) { printf("Hello!\\n"); return 0; }' > /tmp/a.c
% riscv64-unknown-elf-gcc -march=rv64i -mabi=lp64 /tmp/a.c -o /tmp/a
```

Si vous avez ça, vous pouvez vous amuser avec CGDoom.

## Instructions de portage

TODO. Un point important : pas besoin de toucher à `cgdoom`, on se content de modifier dans `src-riscv`.
