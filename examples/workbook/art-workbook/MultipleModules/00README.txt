Homework for Session 12

The input04_all.fcl file runs the TOY Detector simulation as described
in Session 12.

1) cd art-workbook/MultipleModules and create a directory called
"output" (the FCL assumes it is there for output files and will throw
an exception if it is not present).

2) Run the input04_all.fcl file to try it out.  art -c input04_all.fcl

Two files are produced: input04.art (the art output) and input04.root
(the histogram).  Both files go into the output directory.

3) Run the FileDumper as shown on slide 15 on the .art file. Run root
on the .root file and look at it with the TBrowser.

3a) Comment out the end_paths line and run. Note that the end_paths
still get executed.

3b) Comment out both the end_paths line and the e1 line. Note the
warning message and that the end_paths don't run (the .root file will
be empty and the .art file won't get made).

4) Copy input04_all.fcl to input04_all_a.fcl . In the new file, change
the label makeHits to "pizza" (change the name of the output files too
to avoid overwiting old files). Run the FCL and see what errors you
get. Adjust the inputTags and paths to work with the module label
change (makeHits -> pizza). Run the FileDumper to see the change in
the output file. Look at the root file to see how that changed.  The
answer is in answers/input04_all_a.fcl .

5) Slide 18 describes adding a LooseFitter to the workflow. Try that
yourself by copying input04_all.fcl to input04_all_lf.fcl and making
the necessary changes. Run it and check the output with the
FileDumper. The answer is in answers/input04_all_lf.fcl

5a) Adjust input04_all_lf.fcl that you made in step 5) to add another
instance of the massPlot analyzer to plot the LooseFitter tracks. Look
at the resulting root file. The answer is in
answers/input04_all_lf2.fcl

6) Copy input_04_all.fcl to input04_all_splitdet.fcl . In the new file
follow the instructions on slide 20. That is...

Make makeHitsInner and a makeHitsOuter producers, the former only using the
detsim:inner tag and the latter only using the detsim:outer
tag. Adjust the modules and analyzers accordingly (e.g. you will need
fitterInner and fitterOuter). Be sure to change the names of the
output files to avoid overwriting old ones. 

Alter the p1 path to be pInner and create a pOuter path
accordingly. Here's how they should look...

           pInner: [ "evtgen", "detsim", "makeHitsInner", "fitterInner", "randomsaver" ]
           pOuter: [ "evtgen", "detsim", "makeHitsOuter", "fitterOuter", "randomsaver" ]
           trigger_paths: [ "pInner", "pOuter"  ]

Note that the evtgen and detsim modules are in fact not run more than
once. art figures that out.

Adjust the e1 list accordingly. You can add an e2 list that just lists
the new module labels you had to make. Look at the end of the answer file if you get stuck.

Run it with art; run the FileDumper; look at the root file.

The answer is in answers/input04_all_splitdet.fcl .

7) Copy input_04_all.fcl to input04_all_jobA.fcl and
input04_all_jobB.fcl. Follow the instructions on slide 21. That is...

In input04_all_jobA.fcl, remove all of the analyzers to just run the
producers. Remove the TFileService since you won't be making any root
files. Change the output art file name to "input04_jobA.fcl". Note
that you will still need,

e1 = ["outfile"]

to run the output module to make the output file. 

Run input04_all_jobA.fcl with art. Note the creation of output/input04_jobA.art and no root file.

Now edit input04_all_jobB.fcl removing all of the producers. Remove p1
and the trigger_paths. Remove the outputs section (you won't be making
an .art file). KEEP the services below "# From toyExperiment" as some
of the analyzers need them. Change the process name to
"AnalyzeTheYak". Remove everything in source and replace it with,

source: {
          module_type: "RootInput"
          fileNames: ["output/input04_jobA.art"]
        }

Change the name of the output Root file in TFileService to be input04_jobB.root .

Run input04_all_jobB.fcl with art.  Oops - Remove the outfile from the e1 list (Ha Ha) and run again.

Note the creation of input04_jobB.root

THe answer is in answers/input04_all_jobA.fcl and answers/input04_all_jobB.fcl

8) EXTRA ART NINJA CREDIT

Let's combine problems 6 and 7. We'll run reconstruction up to
detsim. Then we'll make two jobs, one for just the inner detector hits
and another job for just the outer detector hits.  This is a nice
example of splitting processing between jobs.

For simplicity I removed the randomsaver from everything, but you
could (and probably should) keep it.

Make a fcl file input04_all_createIntersctions.fcl to run only the
producers evtgen and detsim and make an output art file. That's all it
should do.

Now make a fcl file input04_all_inner.fcl that reads in the .art file
that input04_all_createInstances.fcl outputs and does the rest of the
reconstruction only for the inner detector and makes output plots only
for the inner detector. You should be able to do this without having
to change too many labels (way less than in question 6 above. Make an
output .art file and .root file with "inner" in the file name.

Now make another fcl file input_all_outer.fcl similar to above, but
for the outer detector and making files with "outer" in the file name.

Run it all and check with FileDumper and root.

The answers are in answers/input04_all_createIntersections.fcl,
answers/input04_all_inner.fcl and answers/input04_all_outer.fcl



