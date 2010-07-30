Bitcoin Patchwork client
========================

Notes
-----

* I have only tested this build on Ubuntu 10.04 and only with bitcoind.
* The makefile.unix has -march=native so you will either need to change this or only compile on the system you will be running the binary (or a system with a similar processor).


Current Patchset
----------------

* lachesis's getinfo patch: http://www.alloscomp.com/bitcoin/
* jgarzik's listtransactions v7 patch: http://gtf.org/garzik/bitcoin/patch.bitcoin-listtransactions
* jgarzik's patch to display existing extended-help descriptions: http://gtf.org/garzik/bitcoin/patch.bitcoin-help-extended

Installation
------------

1. Install various required packages
    sudo apt-get install build-essential libgtk2.0-dev libssl-dev libdb4.7-dev libdb4.7++-dev libboost-all-dev

2. Download and extract this repo, or use git to checkout the repo

3. Time to build bitcoind

    make -f makefile.unix bitcoind
