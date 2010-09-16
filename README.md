Bitcoin Patchwork client
========================

Notes
-----

* I have only tested this build on Ubuntu 10.04 and only with bitcoind.

Current Patchset
----------------

* Jgarzik's listtransactions patch: http://www.bitcoin.org/smf/index.php?topic=611.20
* Jgarzik's getblock patch: http://www.bitcoin.org/smf/index.php?topic=724.0
* The listgenerated method, lifted from Lachesis's now deprecated RPC Calls patch: http://www.alloscomp.com/bitcoin/

Custom Modifications:
---------------------

* Changed MAX_OUTBOUND_CONNECTIONS from 8 to 32.
* Disabled receiving of IP transactions by default, they can be enabled with the -allowiptransactions option.

Installation
------------

1. Install various required packages:
> sudo apt-get install build-essential libgtk2.0-dev libssl-dev libdb4.7-dev libdb4.7++-dev libboost-all-dev

2. Download and extract this repo, or use git to checkout the repo.

3. Time to build bitcoind:
> make -f makefile.unix bitcoind
