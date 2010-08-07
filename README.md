Bitcoin Patchwork client
========================

Notes
-----

* I have only tested this build on Ubuntu 10.04 and only with bitcoind.

Current Patchset
----------------

* Lachesis's RPC Calls patch, which also includes jgarzik's listtransactions patch: http://www.alloscomp.com/bitcoin/
* Jgarzik's extended help patch: http://gtf.org/garzik/bitcoin/patch.bitcoin-help-extended

Installation
------------

1. Install various required packages:
> sudo apt-get install build-essential libgtk2.0-dev libssl-dev libdb4.7-dev libdb4.7++-dev libboost-all-dev

2. Download and extract this repo, or use git to checkout the repo.

3. Time to build bitcoind:
> make -f makefile.unix bitcoind
