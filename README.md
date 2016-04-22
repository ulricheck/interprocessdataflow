# interprocessdataflow
Draft of a library for interprocess dataflow applications that allows lock-free data exchange

This repository is a playground for designing a set of components for interprocess dataflow.

The motivation behind this work is to separate hardware drivers from the actual dataflow-based processing.
Such a setup would have the following benefits:

 - On windows platforms it would remove constraints for the main applications based on the compiler requirements of hardware drivers/sdks
 - Programs could be restarted without starting/stopping device communication
 - If any of the components crashes (e.g. hardware drivers) it could be relaunched without affecting the running system (much)..
 
 Be aware - this is a playground - not even alpha software or anything near usable - use at your own risk !!
 
 Ulrich Eck
