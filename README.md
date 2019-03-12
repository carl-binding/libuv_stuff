# Some libuv tests

Pretty much the samples from the [libuv](http://docs.libuv.org/en/v1.x/) tutorials. However, completed
and with some scant comments.

Found the libuv tutorials rather spotty: missing declarations, unclear description of memory allocations and reclaiming.
There are other code samples on WWW; some of them however are coded against older API vesions.

In any event, consider [asychronous programming harmful](https://tqdev.com/2017-10-reasons-async-programming-is-bad). It 
makes sense in a browser WEB GUI which is single threaded, but for real systems programming asynchronous event handling leads to
messy call-backs, confused memory management and tangled code, IMHO.


