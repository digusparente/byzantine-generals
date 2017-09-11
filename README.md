# Byzantine Generals

The Byzantine General's Problem is one of many in the field of agreement protocols.

This problem is built around an imaginary General who makes a decision to attack or retreat, and must communicate the decision to his lieutenants.
A given number of these actors are traitors (possibly including the General.) Traitors cannot be relied upon to properly communicate orders;
worse yet, they may actively alter messages in an attempt to subvert the process.

In general, a solution to an agreement problem must pass three tests: *termination*, *agreement*, and *validity*.
As applied to the Byzantine General's problem, these three tests are:


    * A solution has to guarantee that all correct processes eventually reach a decision regarding the value of the order they have been given.
    * All correct processes have to decide on the same value of the order they have been given.
    * If the source process is a correct process, all processes have to decide on the value that was original given by the source process.

Note that one interesting side effect of this is that if the source process is faulty, all other processes still have to agree on the same value.
It doesn't matter what value they agree on, they simply all have to agree. So if the General is subversive, all lieutenants still have to come to a common,
unanimous decision.