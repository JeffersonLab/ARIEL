#!/usr/bin/env python
from __future__ import print_function

import os
import ROOT

ih1 = ROOT.r10870.Handle['int']()
iwf = ih1.whyFailed()
print("Default-constructed Handle<int>")
print("- Type: {0}".format(type(iwf)))
print("- Boolean value: {0}".format(bool(iwf)))

doh1 = ROOT.r10870.Handle[ROOT.r10870.DataObj]()
dowf = doh1.whyFailed()
print("Default-constructed Handle<DataObj>")
print("- Type: {0}".format(type(dowf)))
print("- Boolean value: {0}".format(bool(dowf)))
