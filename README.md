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

### CORS header 'Access-Control-Allow-Origin' missing

When using the browser application, the API request could be denied with the return 'Reason: CORS header 'Access-Control-Allow-Origin' missing'.
In that case, one needs to set the Access-Control-Allow-Origin header's value as explained [here](https://developer.mozilla.org/en-US/docs/Web/HTTP/CORS/Errors/CORSMissingAllowOrigin).

If you use the docker setup of Hornet just add 

```
- "traefik.http.middlewares.cors.headers.customResponseHeaders.Access-Control-Allow-Origin=https://eddytheco.github.io"
- "traefik.http.routers.hornet.middlewares=cors"
```
to docker-compose.yml in the traefik section. Such that browser API requests from https://eddytheco.github.io are accepted  by your node.
