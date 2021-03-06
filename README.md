Generic data mixing framework, R. A. Bertens (UTK,CERN) 2018

This is a short instruction manual on how to use the event mixing framework, serving to complement the analysis note, and to be used as guidance for anyone who wants to use the framework. 

It's recommended to read the full manual before starting with the package, as the manual also motivates why the package is organized the way it is. 

# Branches
The current project has three branches

- **master branch**: always supposed to work and to be up-to-date
- **legacy branch**: a branch that only exists to run on an mixed events that were created prior to September 2018, to mitigate a bug in the compression settings (should have been a tag, but choices were made)
- **migrate-to-cmake**: moves the project out of needing CINT/CLING to launch to using CMake, only supports mixing (not jet finding)

In short, just use the master branch and all should be well. 

# Prerequisites and goal

The package contains three different modules, which are built only when invoked to run. Each module has a different purpose and different prerequisites. 

### Data filtering
In this step, full ALICE events (AODs) are filtered, event and track selection is performed, and only information that is necessary for the final analysis is written into mini-events, which subsequently are written into a TTree. More details are given in the 'running' section of this manual. Since this step requires reading of AODs, a full AliPhysics software stack is necessary. If AODs are resolved from the GRID, a valid authentication token is necessary as well. 

Data filtering can be started with the scripts
- runTTreeFilter.C (launches a filtering of locally available AODs)
- runTTreeFilterOnGrid.C (launches a filtering on the GRID)
- runTTreeFilterOnGridROOT6.C (same as the above, but working with CLING)

### Event mixing
Event mixing takes the mini-events from the simple TTrees, performs event mixing (details are given in the 'running' section), and writes out mixed events into TTrees, using the same mini-event format. Event mixing has as only prerequisite an installation of ROOT

Event mixing can be started with the script
- runEventMixer.C

### Jet finding
To facilitate jet finding on the mini-events, a jet finding implementation is available, which depends on both ROOT and FastJet. Exporting the standard and ROOT FastJet variables suffices to perform jet finding (a build of AliPhysics is not necessary). 

Scripts that are available comprise:
- runJetFindingOnMixedEvents.C (does jet finding on mixed events)
- runJetFindingOnTree.C (does jet finding on TTree mini events)

Except for some flags that are set, there is no fundamental difference between performing jet finding on mixed or unmixed mini-events: both of the above scripts use the same jet finding class. 

# Automatic documentation
Minimal automatic documentation pages can be generated using doxygen. To generate the documentation (in html and pdf form), from the source directory of the framework package do

```
doxygen doxygen.config
```

Generating the documentation requires an installation of doxygen on your system. 

# Running the package

This section is divided into three parts

- data filtering
- event mixing
- jet finding

This section gives a (brief) overview of how to run these three different modules. 

## Data filtering

Full events (AODs) are too large to mix efficiently, moreover, they contain a lot of information that is not relevant to the final data analysis. The point of data filtering is to

- perform event selection
- perform track selection
- filter out relevant information from selected events and tracks
- store this information in mini-events
- perform an optional, lossy compression

Data filtering can be started with the scripts

- runTTreeFilter.C (launches a filtering of locally available AODs)
- runTTreeFilterOnGrid.C (launches a filtering on the GRID)
- runTTreeFilterOnGridROOT6.C (same as the above, but working with CLING)

The actual configuration of the filtering procedure is governed by the 'AddTask' macro, which can be found under `add_task_macros/AddTaskTTreeFilter`. Filtering options, for both events and tracks, are specified by calling set function on event track selection classes, e.g.

```cpp
    // initialize task, connect it to the manager and return it
    AliAnalysisTaskTTreeFilter* filter = new AliAnalysisTaskTTreeFilter("filter");
    // add the task to the manager
    mgr->AddTask(filter);
    // set the trigger selection
    filter->SelectCollisionCandidates(trigger);

    // do not store QA on the grid runs
    filter->SetDoQA(kFALSE);
    // create the event and track cut objects
    AliGMFEventCuts* eventCuts = new AliGMFEventCuts();
    filter->SetEventCuts(eventCuts);
    
    AliGMFTrackCuts* trackCuts = new AliGMFTrackCuts();
    trackCuts->SetFilterBit(768);
    filter->SetTrackCuts(trackCuts);
```

### Event format - AliGMFTTreeHeader
Events are written away into a TTree as mini-events. These mini events comprise an event header, and a track array, later it will be shown that these events are most easily accessed via a wrapper class called a container. 

The event header derives from TObject and  is specified as follows

```cpp
#ifndef COMPRESSION_LEVEL
#define COMPRESSION_LEVEL 1
#endif

...

class AliGMFTTreeHeader : public TObject{

 public:

  AliGMFTTreeHeader();
  void  Fill(AliGMFTTreeHeader* event);
  
  // manipulators - persistent
  void  SetZvtx(Float_t Zvtx)           {fZvtx  = Zvtx;}
  void  SetEventPlane(Float_t ep)       {fEventPlane = ep;}
  void  SetEventPlane3(Float_t ep)      {fEventPlane3 = ep;}
  void  SetRunNumber(ULong_t rn)        {fRunNumber = rn;}

  // manipulators - no persistent
  void SetEventID(Int_t id)             {fEventID = id;}
  void SetUsed(Bool_t used)             {fUsed = used;}
  void SetMultiplicity(Short_t m)       {fMultiplicity = m;}
  void SetCentrality(Double_t c)        {fCentrality = c;}


  // getters
  Float_t       GetZvtx() const         {return fZvtx;}
  Float_t       GetEventPlane() const   {return fEventPlane;}
  Float_t       GetEventPlane3() const  {return fEventPlane3;}

  Int_t         GetEventID() const      {return fEventID;}
  Bool_t        GetUsed() const         {return fUsed;}
  Short_t       GetMultiplicity() const {return fMultiplicity;}
  Float_t       GetCentrality() const   {return fCentrality;}
  ULong_t       GetRunNumber() const    {return fRunNumber;}

  void          Reset();
 private:
  // first the persistent members are listed. these are written to disk
  // so extra care is taken to minimize the space they take
#if COMPRESSION_LEVEL > 1
  // maximum compression, some loss of precision may occur
  Double32_t    fZvtx;          //[-10,10,8] rec vertex
  Double32_t    fEventPlane;    //[-1.6, 1.6,8] event plane orientation
  Double32_t    fEventPlane3;   //[-1.05, 1.05,8] event plane 3 orientation
  Double32_t    fCentrality;    //[0,100,8] collision centrality
#elif COMPRESSION_LEVEL > 0
  //medium compression, no precision loss expected
  Double32_t    fZvtx;          //[-10,10,12] rec vertex
  Double32_t    fEventPlane;    //[-1.6,1.6,12] event plane orientation
  Double32_t    fEventPlane3;   //[-1.05, 1.05,12] event plane 3 orientation
  Double32_t    fCentrality;    //[0,100,12] collision centrality
#else  
  // no compression
  Float_t       fZvtx;          // rec vertex
  Float_t       fEventPlane;    // event plane orientation
  Float_t       fEventPlane3;   // event plane 3 orientation
  Float_t       fCentrality;    // collision centrality
#endif

  // non-transient non compressable numbers
  ULong_t       fRunNumber;     // run number
  // transient members are not written to disk, they
  // can be optimized for speed
  Int_t         fEventID;       //! event identifier
  Bool_t        fUsed;          //! was event read from file
  Short_t       fMultiplicity;  //! event multiplicity
```

Crucial in this header is the COMPRESSION_LEVEL, which governs the level of compression of streamable class members:

- COMPRESSION_LEVEL=0 results in no compression
- COMPRESSION_LEVEL=1 performs lossy compression, but for the defined variables, loss of information is negligible w.r.t. resolution of the members
- COMPRESSION_LEVEL=2 performs very aggressive compression that may lead to a destructive loss of information

In general, COMPRESSION_LEVEL=1 is the safe choice. Compression level has to be known at compile-time, as it changes the streamer definition of the object. **The same compression level has to be used when reading in the events** to ensure streamer compatibility. There is no way to retrieve the compression level with which a data set was generated though, so be advised to not change this often, or to bookkeep very diligently what you were running. 

### Track format
Tracks are written as mini-tracks into a TClonesArray. These mini-tracks have the name AliGMFTTreeTrack, and are built up as follows

```cpp
#ifndef COMPRESSION_LEVEL
#define COMPRESSION_LEVEL 1
#endif

#ifndef AliGMFTTreeTRACK_H
#define AliGMFTTreeTRACK_H

#include <TObject.h>

class AliGMFTTreeTrack : public TObject {

    public:

        AliGMFTTreeTrack();
        void       Fill(AliGMFTTreeTrack* track);
        void       Fill(Double_t pt, Double_t eta, Double_t phi, 
                Short_t charge, Bool_t used, Int_t number) {
            fPt = pt;
            fEta = eta;
            fPhi = phi;
            fCharge = charge;
            fUsed = used;
            fNumber = number; 
        }

        void       SetPt(Float_t pt)                {fPt =          pt;}
        void       SetEta(Float_t eta)              {fEta =         eta;}
        void       SetPhi(Float_t phi)              {fPhi =         phi;}
        void       SetCharge(Short_t charge)        {fCharge =      charge;}
        void       SetUsed(Bool_t used)             {fUsed =        used;}
        void       SetNumber(Int_t number)          {fNumber =      number;}

        Float_t    GetPt() const                    {return fPt;}
        Float_t    GetEta() const                   {return fEta;}
        Float_t    GetPhi() const                   {return fPhi;}
        Float_t    GetCharge() const                {return fCharge;}
        Bool_t     GetUsed() const                  {return fUsed;}
        Bool_t     GetFilled() const                {return fFilled;}
        Int_t      GetNumber() const                {return fNumber;}

        void       Reset();
        void       PrintInfo();
    private:
#if COMPRESSION_LEVEL > 1
        // maximum compression level
        Double32_t       fPt;       //[0,100,8]
        Double32_t       fEta;      //[-1,1,8]
        Double32_t       fPhi;      //[0,6.3,8]
#elif COMPRESSION_LEVEL > 0
        // medium compression level
        Double32_t       fPt;       //[0,100,12]
        Double32_t       fEta;      //[-1,1,12]
        Double32_t       fPhi;      //[0,6.3,12]
#else
        // no compression
        Double_t       fPt;         //[0,100,8]
        Double_t       fEta;        //[-1,1,8]
        Double_t       fPhi;        //[0,6.3,8]
#endif
        Short_t          fCharge;   //charge of track

        // some transient members that we'll use for bookkeeping, but dont want to store now
        Bool_t        fUsed;        //! was track used for mixing ? 
        Bool_t        fFilled;      //! was track filled ?
        Int_t         fNumber;      //! just an int - use as you see fit 
```

The same words of caution w.r.t. the COMPRESSION_LEVEL as given in the previous subsection are relevant here. A few utility function are available in this class as well (e.g. various `Fill` functions). 

### Final filtered data

It is worth to note, that it's probably wise to create a large number of 1 GB large output files containing merged events, rather than writing out one enormous file. While the latter is possible (the output is not buffered in RAM but written to disk on the fly), having multiple small files allows for more efficient distribution of subsequent tasks (e.g. jet finding) on a batch farm. Creating e.g. one output file per run number is a good way to go. 

## Event mixing

The heart of this package is the event mixing routine. Event mixing is steered by the macro `runEventMixer.C`. Input arguments to supply are

- input files to run over (in the mini-event format)
- mixing event selection criteria
- technical configuration of the mixer

The mixing is carried out by the class AliGMFMixingManager, which can, if instructed, also store QA histograms regarding the mixing. The following subsections detail the above three input that needs to be specified. 

##e Input files to run over
Input files to run over are simply passed to the mixing manager in the form of a `AliGMFEventReader`, which in turn takes a `TChain` of mini-events as input. To initialize the reader with a `TChain` called `myChain` and connect it to a new mixing manager, simply do

```cpp
    AliGMFEventReader* reader = new AliGMFEventReader(myChain);
    // create the mixer and connect the input event reader
    AliGMFMixingManager* mixer = new AliGMFMixingManager();
    mixer->SetEventReader(reader);
```

The event mixer will use all events that are available in the chain for mixing, the user does not have to define an explicit event loop. 

### Mixing event selection criteria
Tracks from events that have similar characteristics are used for mixing. These characteristics are
- multiplicity
- vertex z position
- event plane angle
- third order event plane angle
- centrality

For all these characteristics, intervals can be set by doing

```cpp
    // configure the mixer
    mixer->SetMultiplicityRange(minMult, maxMult);
    mixer->SetVertexRange(minVtx, maxVtx);
    mixer->SetEventPlaneRange(minEp, maxEp);
    mixer->SetEventPlane3Range(minEp3, maxEp3);
    mixer->SetCentralityRange(minCen, maxCen);
```

Smaller intervals result in more precise results, but will require large data samples. 

### Technical configuration of the mixer

Here comes the fun part. Event mixing is challenging, and you can configure the mixer in such a way both efficiency and efficacy are ensured. The goal of the event mixer is simple: **create mixed events that have the same multiplicity distribution as their unmixed counterparts, and ensure that each track of a mixed event is picked from a different unmixed event**. 

The way the mixer works, is as follows. Take the simple example of wanting to mix tracks with a multiplicity between two and three, then it sets N to 3 (the maximum desired multiplicity). The mixer will read through the input chain, and fill an NxN matrix with tracks that it finds. Let's call these tracks S0T0 through SNTN, where the SN means unmixed event N, and TN track number N from that event. A matrix filled with its rows filled with two events with a multiplicity of 3 and one event with a multiplicity of 2 could look like

| ME0|ME1|ME2|
|---|---|---|
| S0T0 | S0T1 | S0T2 |
| S1T0 | S1T1 |  |
| S2T0 | S2T1 | S2T2|

The columns of this matrix are labeled MEN, since we will fill N mixed events from them. Mixed events are now creating by finding vertical paths through this matrix (columns), and putting the encountered tracks into new mixed events. From the above example, we can construct three mixed events, the multiplicity of the ensemble is automatically preserved:

- ME0: S0T0, S1T0, S2T0
- ME1: S0T1, S1T1, S2T1
- ME2: S0T2, S2T2

Note, that there is a non-trivial step that is performed before actually constructing the mixed events: the positions of the elements of each row are randomized. This randomization is performed because the ordering of tracks within ALICE events is (somewhat) ordered according to transverse momentum, therefore not performing a shuffling would lead to an unbalanced momentum distribution. This procedure is carried out by efficient randomization of a standard vector with integer values representing row elements, so no shuffling of allocated memory is performed for reasons of efficiency. 

When dealing with less trivial examples, it is not obvious a solution can be found when following the same approach

| ME0|ME1|ME2|ME3|ME4|
|---|---|---|---|---|
| S0T0 | S0T1 | S0T2 | S0T3 | S0T4 |
| S1T0 | S1T1 | S1T2 | S1T3 | S1T4 |
| S2T0 | S2T1 | S2T2|  |  |
| S3T0 | S3T1 | S3T2|  |  |
| S4T0 | S4T1 | S4T2|  |  |

To construct mixed events with proper multiplicity from columns, a shuffling is performed, in which the matrix is rearranged like

| ME0|ME1|ME2|ME3|ME4|
|---|---|---|---|---|
| S0T0 | S0T1 | S0T2 | S0T3 | S0T4 |
| S1T0 | S1T1 | S1T2 | S1T3 | S1T4 |
| S2T0 | S2T1 |  S2T2|  |  |
| S3T0 | S3T1 |  |  S3T2|  |
| S4T0 | S4T1 |  |  | S4T2 |

#### Buffer padding

Some examples do not have a solution, e.g.

| ME0|ME1|ME2|ME3|ME4|
|:-:|:-:|:-:|:-:|:-:|
| S0T0 | S0T1 | S0T2 | S0T3 | S0T4 |
| S1T0 | S1T1 | S1T2 | S1T3 | S1T4 |
| S2T0 | S2T1 | S2T2| S2T3 |S2T4 |
| S3T0 | S3T1 | S3T2| S3T3 |S3T4 |
| S4T0 | S4T1 | S4T2|  |  |

To make sure that we do not mix more than one track from a given unmixed into event into a mixed event, tracks can only be shuffled to different columns, not different rows. If you observe that mixed multiplicity distributions do not match up with the multiplicity distribution of the unmixed input events (all distributions are available in the mixing QA files), we can add a `buffer column` to the mixing matrix. This is done by specifying

```cpp    
mixer->SetAllowBufferPadding(bufferPadding);
```
where `bufferPadding` is an integer percentage, which tells the mixing manager which percentage of N (the maximum multiplicity) events, should be added as buffer columns in the mixing matrix, e.g. when N = 10, and bufferPadding = 10, a mixing matrix with 11 columns (0.1 times 10) and 10 rows is created. 

Usually, a bufferPadding of 5 or 10 suffices; note though that using a buffer comes at an efficiency loss, but when staying between 5 and 10, the efficiency loss from adding the buffer tends to be (much) smaller than the efficiency loss that occurs naturally from mixing matrices that do not have a solution. 

#### Overflow

Another way in which the mixer can fail, is when it is not able to fill a complete NxN mixing matrix, one wants to e.g. look for events with a multiplicity between 2 and 3, but only 2 events qualify. Let's for convenience say, that these events were number 10 and 11 that were found in the input chain (so events 0 through 9 have been used in previous mixing matrices), but event 11 is the last event in the chain

| ME0|ME1|ME2|
|---|---|---|
| S10T0 | S10T1 | S10T2 |
| S11T0 | S11T1 | S11T2 |
|  | | |

Unless otherwise specified, the mixer will exit when a mixing matrix cannot be filled, and the tracks in the matrix are lost. 

However, the mixer can also be instructed to skip to reading input chain of events from event 0 again, and fill the matrix so that it looks like

| ME0|ME1|ME2|
|---|---|---|
| S10T0 | S10T1 | S10T2 |
| S11T0 | S11T1 | S11T2 |
| S0T0  | S0T1 | S0T2 |

Note the last row. After this, mixed events are still created according to the **original multiplicity specifications**, so two events with a multiplicity of 3 would be created here, with as content

- ME10: S10T0, S11T0, S0T0
- ME11: S11T0, S11T1, S0T1

Using this overflow option can be toggled on and off by passing a boolean argument `AutoOverflow` via the setter

```cpp
    mixer->SetAutoOverflow(AutoOverflow);
```

Using the overflow option is safe, in the sense that the mixer will still create mixed events in which each track is drawn from a different unmixed event (even though some tracks are recycled). Only when the dimensionality of the matrix is very large, and the number of input event candidates is very small, can the situation occur that the full input chain is read more than once when trying to fill a mixing matrix. In this unlikely event, the same track can appear in the matrix more than once, and, in theory, end up twice in the same event. However, since the indices of each row are randomized prior to the actual mixing (a necessary step, since track reconstruction orders tracks roughly according to momentum), double usage of tracks is extremely unlikely. 


#### Miscellaneous 
In order for mixed event output files to not be too large, output can be split up into multiple files. The maximum number of events can be set by calling

```cpp
    mixer->SetMaxEventsPerFile(500);
```

for any number of events per file. Note that mixing matrices are written out as a whole, so if file A is open and the maximum number of events is reached, the content of the full mixing matrix will still end up in file A. Therefore, by calling SetMaxEventsPerFile(x), the number of events that ends up in an output file can still be x+N, where N is the number of rows in the mixing matrix. 

In addition, to generate small batches of mixed events for e.g. testing, the mixer can be instructed to exit after a certain number of events is generated by calling

```cpp
    mixer->SetMaxEvents(10000);
```

the same boundary conditions apply here as for setting the maximum number of events per file. 

To store QA information on the mixing itself, call

```cpp
    mixer->DoQA();
```

This will bookkeep QA information on track-by-track and ensemble features of the events, and write the info out to a separate file (separate from the mini-events that are written out). Histogram manipulation is delegated to the general histogram manager class that is available in the package, which uses an efficient hashed list to call histograms. 

To get an idea of the program flow of the mixing manager, you can set the verbosity level, defined by preprocessor flag `VERBOSE`, to 1 (or higher), in the class header prior to compilation. 

## Jet finding

The package ships with its own jet finding class, which relies on FastJet to perform jet finding. Jet finding is performed by the aptly called `AliGMFSimpleJetFinder` class. 

### Running the jet finder on unmixed events

To run the jet finder on unmixed events, one can invoke the macro `runJetFindingOnTree.C`. To read events, the jet finder relies on an event reader that is initialized with a TChain of mini events. After initializing a jet finder (multiple instances can be initialized to operate on the same reader, to e.g. facilitate analyzing jets with different radii, one can pass event and track cut objects to the jet finder via setter functions

```cpp
     // create the event cuts
    AliGMFSimpleEventCuts* eventCuts = new AliGMFSimpleEventCuts();
    eventCuts->SetCentralityRange(cenMin, cenMax);
    AliGMFSimpleTrackCuts* trackCuts = new AliGMFSimpleTrackCuts();
    trackCuts->SetTrackMinPt(minConstPt);

    jetFinder[i]->SetEventCuts(eventCuts);
    jetFinder[i]->SetTrackCuts(trackCuts);
```
Since the data that is used as input is already filtered, using these cuts is not strictly necessary. When the objects at not set, no selection is performed. 

When browsing through the `runJetFindingOnTree.C` macro, one can find a set of options that is passed through the jet finders. They are explained here below, but have sane defaults. 

```cpp
       jetFinder[i] = new AliGMFSimpleJetFinder();

       // set the resolution parameter for the jet finder (anti-kt)
       jetFinder[i]->SetJetResolution(radii[i]);

       // set the resolution parameter that is used for the background estimator (kt)
       jetFinder[i]->SetJetResolutionBkg(radii[i]);

       // the following options are not really relevant for unmixed event analysis

       // optional: tracks with a pt exceeding a certain value can be split into multiple fragments
       // this is only useful for running on mixed events, in the run macro for unmixed events
       // these setters are only shown for illustrative purposes
       jetFinder[i]->SetSplittingForTracksWithPtHigherThan(splitTracksFrom);

       // leading hadron pt requirement
       jetFinder[i]->SetLeadingHadronPt(leadingHadronPt);

       // 'inverted' leading hadron pt requirement
       jetFinder[i]->SetLeadingHadronMaxPt(leadingHadronMaxPt);

       // reject N hard jets for the background estimation
       jetFinder[i]->SetRejectNHardestJets(rejectNJ);

       // toggle on or off a random cone dpt analysis (slows down the procedure)
       jetFinder[i]->SetDoRandomConeAnalysis(kTRUE);

       // initialize the jet finders
       jetFinder[i]->Initialize();

```

The above information and the `runJetFindingOnTree.C` macro should give sufficient information on how to run the jet finders. 

### Jet finding on mixed events

The procedure for running the jet finders on mixed events is similar to running on unmixed events. The macro `runJetFindingOnMixedEvents.C` can be used to steer jet finding on mixed events. An important feature of the jet finder that pertaining to running on mixed events, is the treatment of tracks with high transverse momentum. These tracks can be split into multiple fragments, where the sum of the fragments' transverse momentum equals the initial transverse momentum of the highly energetic track. 

There are several ways to change the splitting behavior (and note that splitting can be done both for unmixed and mixed events), the setters to govern the procedure do not have the clearest names, and are defined as follows

```cpp
        void    SetSplittingForTracksWithPtHigherThan(Double_t pt) {
            fSplittingThreshold = pt;
        }
```
The above method can be used to set a splitting threshold: tracks with a transverse momentum exceeding this threshold will be split into multiple fragments. 


```cpp
        void    SetSplitTrackPt(Double_t pt) {
            fSplitTrackPt = pt;
        }
```
This method has an unfortunate name, that is here for legacy reasons. If you set it to a value **lower than 0**, the transverse momentum of a track that exceeds the threshold value is just set to the threshold value (i.e., if the threshold value is 3, and a track has a transverse momentum of 10.7, its momentum will just be changed to 3, and the remaining 7.3 GeV/c is lost).

Conversely, if you specify a value higher than 0, an iterative splitting procedure is started, were a random transverse momentum is sampled from a realistic distribution which is defined between 0.15 and fSplittingThreshold. A new track is created with this sampled transverse momentum.  The original transverse momentum of the track is decreased by this sampled value. If the new value still exceeds the threshold, the process is repeated, until a set of new tracks (in the code referred to 'mocked up tracks'), which all obey the threshold value, is obtained. These tracks can then distributed over multiple mixed events: each split fragment is embedded into a different mixed event. 

To split the fragments into multiple events, call

```cpp
        void    SetCollinearSplittingOverMEs(Bool_t r) {
            fCollinearSplittingOverMEs = r;
            fRandomizeSplitTrack = kFALSE;
        }
```

Since this procedure results in a 'pool' of tracks that have to be distributed into several mixed events, a sophisticated event procedure is started when this option is chosen. The first 5 analyzed events are just used to build up a pool of track fragments. For threshold values as low as 2, this is a reasonable number of events to build up a stable buffer. From the 5th event on, analysis proceeds as normal, embedding fragments from previous events into current mixed events on the fly. When the last event in the loop has been analyzed, the first 5 events are re-analyzed: this time, they undergo a full analysis, and use the existing track fragment buffer (which contains fragments of the events at the end of the chain) for embedding of fragments. In this way, all data are analyzed.


Otherwise, the fragments can be embedded into the event from which they originate. To govern this procedure, two methods exist

```cpp
        void    SetRandomizeSplitTrackEtaPhi(Bool_t r) {
            fRandomizeSplitTrack = r;
        }
        void    SetPreserveSplitTrackPhi(Bool_t r) {
            fPreserveSplitTrackPhi = r;
        }
```
These two methods are here for complicated reasons: they give the user the possibility to randomize eta and phi values of the track that is truncated. By toggling the first on to true, randomization is enabled. By setting the second one to true, the azimuthal angle of the track is preserved. If one splits tracks, but does not randomize, the entire procedure of splitting is pointless: collinear safety of the jet finder ensures that the results with and with out splitting are the same. 



# Job submission scripts

Nobody wants to submit thousands of jobs by hand, therefore, a collection of scripts to automatically submit jobs (using bsub, but these can easily be used as a template for other queue systems), can be found under the `scripts/job_handling` folder. Scripts are divided into two categories:

- `kickstart*` which call `bsub*` scripts with different parameters
- `bsub*` scripts, which themselves write a shell script that contains the actual job description (and is also launched at the end of the bsub script)

The bsub scripts create a temporary directory under /tmp, where jobs are executed. Output data is, at the end of the job, copied to the directory from which the job is launched. These bsub scripts contain some hardcoded paths that likely need to be changed when you run on your own system, but changes should be straightforward. 

The folder `scripts/postprocessing` shows some example scripts that can be used to compare mixed and unmixed event analysis results. 
