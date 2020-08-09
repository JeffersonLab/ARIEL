Exercises for creation and direct use of Assns.
-----------------------------------------------

1. Load PastimeAssnsProducer_module.cc into an editor.

2. Make and fill an awb::PastimeSurveyData (see PastimeSurveyData.hh) for placement in the art::Event. Use the information to be found in PastimeDataProducer_module.cc (the comments after each element of the PersonalPastimeDataSet give the associations) and the headers in this directory as appropriate. Don't forget to put the filled product into the event. Your first addSingle call should be something like:

    psd->addSingle( { people, 1 }, // art::Ptr<awb::Person>
                    { pastimes, 5 }, // art::Ptr<awb::Pastime>
                    *dataIter++ // awb::PersonalPastimeData
                  );

3. Save your plugin.

4. Load PastimeAssnsAnalyzer_module.cc into an editor.

5. Using the surveyData assocation object, ascertain how many pastimes enjoyed by the survey population cost more than costThreshold per month; store the answer in expensiveCount.

6. Create a suitable FindMany object and use it to ascertain how many people in the survey population have in excess of nPastimesThreshold pastimes; store the answer in nHedonists.

7. Save your plugin.

8. Compile your plugins, and find your answers.
