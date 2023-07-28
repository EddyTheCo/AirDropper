# [AirDropper](https://eddytheco.github.io/AirDropper/index.html)

This repo produce an application that can transfer crypto coins on networks of IOTA with stardust protocol.

In order to use the application one needs to set the address of the node to connect.
The Proof of Work has to be performed by the node (by setting the JWT for protected routes, by enabling PoW in the node...).
By default the application connects to a testnet node that performs PoW for the client.
In principle it will also work for mainnets by setting the node to a mainnet one. 
**Make sure to test and understand how it works before using it on the mainnet**.


The application produce a random seed to be able to sign blocks using the 0 index address.
The application is not intended to store funds on that seed.
One could also save the produced seed just in case.

