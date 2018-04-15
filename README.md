# SIPO
OVERVIEW
A Seismic Insurance portfolio Optimizer, developed in C++, based on a simple seismic catastrophe model, consistent with earthquake engineering priciples and a stochastic model to simulate the earthquake uncertainity. for the purpose of optimization, a meta-heuristic method, called SPSA, has been used. This project is based on the reseaches performed in dissertaion in K.N.T.U.; and for the time, all right is reserved.

BACKGROUND
Insurance is supposed a profittable activity, but in catastrophe events this is not a certainity as we are dealing with an chain of uncertain event. Such Events e.g. earthquakes require a different approach, using catastrophe models. So Seismic Insurance Portfolio Optimization benefits from a catastrophe loss estimationn model using the basic princip;es of earthquake Engineering. A monopoly market is assumed, so an INSURER object could be made by INSURER class which uses data from loss estimation module, INVENTORY. Meanwhile the sequence of seismic event are an important issue of the feasibility of inusrance liability portfolio. Thus another module, regarding the stochastic charactristics of the events is developed. This modules, wrapped in diffrent C++ class perform a systematic evaluation of insurance policy effect on seismic insurace portfolio liability under the all possible stochastic scenarios. Then a stochastic optimizatio module optimizes the descision variables, policy conditions, in order to obtain the possible maximum amount of target function.

How To USE.
To be added soon

Settings
To be Added soon

Compile
The current project has been compiled using mingw++ compiler whithin CODEBLOCKS in Windows 7. I don't expect that such a project not to be compiled with the same settings in Linux, but surely it won't be compiled isng Microsoft Visual Studio. I will try to make it compatible in future, but there will be a little of guarantee.

ACKNOWLEDGE
My sincere regards towards Dr. M.R.Zolfaghari(Faculity member of KNTU) who supervised this project.
