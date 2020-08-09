Exercises for Assns and its Smart Query Objects.
------------------------------------------------

1. Load the AssnsSQO_module.cc into your editor.

2. In the analyzer() function, Obtain a tex::IntersectionCollection from the event by modifyng the getValidHandle() arguments to use the correct tag obtained from the configuration.

3. Use the collection as a reference collection to initialize foTrkHits using the correct arguments to its constructor.

4. Verify that the art::FindOne is valid, and that its size is the same as that of the reference collection used.

5. Ascertain the number of intersections that have a corresponding tex::TrkHit matched to them in each event.

6. Save and compile your plugin and use the provided .fcl file to test your code with the provided data (e.g.):

    art -c AssnsSmartQueryObjects/assnsSQO.fcl -s inputFiles/input01.art

7. If you have time, add to your module by creating a art::FindMany (or art::FindManyP) and verifying that the size of the returned vector for each association is 1 or zero corresponding to whether the FindOne association is valid or not.

8. If you have time, run the same job using the detsim:outer intersections instead of detsim:inner. Note that the different sets of data are being retrieved from the same Assns data product.
