# Alsa and tinyalsa plugins for the parameter-framework

These are plugins for the
[parameter-framework](https://github.com/01org/parameter-framework)
which allow to address alsa mixers and alsa devices.


## Compiling

You need to install the parameter-framework libraries
and headers first (see the parameter-framework's
[README](https://github.com/01org/parameter-framework/blob/master/README.md)).

Generate the Makefiles with `cmake .` and build with `make`.
If you installed the parameter-framework in a custom directory, you should add a
`-DCMAKE_PREFIX_PATH=/path/to/custom/install` argument to `cmake` e.g:
`cmake -DCMAKE_PREFIX_PATH=/home/myself/dev/pfw .` .

If you want to install the plugin to a custom directory, you can add a
`-DCMAKE_INSTALL_PREFIX=/path/to/install/dir` argument to `cmake` e.g:
`cmake -DCMAKE_INSTALL_PREFIX=/home/myself/dev/pfw .` .

Finally, install the libraries with `make install` .

Note that **only the alsa plugins are built**, since tinyalsa's use outside of
Android seems very limited.


## Prerequisites
* Alsa C++ driver access library (the `libclalsadrv2` package on Ubuntu)
* Alsa utilities (the `alsa-utils` package on Ubuntu) for running the example.
* An installed version of the [parameter-framework](https://github.com/01org/parameter-framework)


## Example
In this example, we are going to change the master volume of our Linux system.

We can observe the volume change with the `alsamixer` command.
Here is an output example of our initial volume, which is at `minimum value`:

![AlsaMixer initial view](http://01org.github.io/parameter-framework/hosting/alsamixer_0volume.png "AlsaMixer initial view")


### Prerequisites
Since the structure files are hardware dependant, The .xml files from the
[samples](https://github.com/01org/parameter-framework-samples) will not *just work*
for your hardware configuration.

The **card name** and the **control name** are hardware specific information we need to control the
volume of our Linux desktop via the parameter-framework.

#### Alsa card name
To find the card name, just read the content of `/proc/asound/cards`.
Usually, you want the first card available.
In the example, the card name is `PCH`.

#### Alsa Control name
To find the control we need, we can use the `amixer` utility.
The `amixer controls` command shows every controls we can use via Alsa.
In the example, the control we are using is `'Master Playback Volume'`.

#### Parameter information
Now that we have the control name we are interested in, we can get some
information about the parameter we need to describe. When typing:

    amixer -c0 cget name='Master Playback Volume'

We get the following result:

    numid=19,iface=MIXER,name='Master Playback Volume'
      ; type=INTEGER,access=rw---R--,values=1,min=0,max=64,step=0
      : values=64
      | dBscale-min=-64.00dB,step=1.00dB,mute=0

This can be used to retrieve useful information, such as:

* The **type** (In the example the parameter is of *Integer* type)
* The **min/max** values (In the example, the min=*0* and the max=*64*)


### Structures files used in the example

Now that we have the information we need, it is possible to fill in the required structure files.

#### ParameterFrameworkConfiguration.xml
In this file, just update the *Location Folder*, so that it points towards the
directory where you installed the plugin.

```xml
<?xml version="1.0" encoding="UTF-8"?>
<ParameterFrameworkConfiguration xmlns:xsi="http://www.w3.org/2001/XMLSchema-instance"
    xsi:noNamespaceSchemaLocation="Schemas/ParameterFrameworkConfiguration.xsd"
    SystemClassName="Audio" TuningAllowed="true" ServerPort="5000">
    <SubsystemPlugins>
        <!-- Path to your plugin folder, where the alsa plugin is installed -->
        <Location Folder="<path/to/your/plugin/directory/>">
            <Plugin Name="libalsa-subsystem.so"/>
        </Location>
    </SubsystemPlugins>
    <StructureDescriptionFileLocation Path="AudioStructure.xml"/>
    <!--
    <SettingsConfiguration>
        To go further and specify Settings, visit the wiki page.
    </SettingsConfiguration>
    -->
</ParameterFrameworkConfiguration>
```

#### AudioStructure.xml
This file is generic and does not need to be updated.
If we have two sound cards, we could eventually *include* them both here.

```xml
<?xml version="1.0" encoding="UTF-8"?>
<SystemClass xmlns:xsi="http://www.w3.org/2001/XMLSchema-instance"
    xsi:noNamespaceSchemaLocation="Schemas/SystemClass.xsd"
    Name="Audio">
    <SubsystemInclude Path="AlsaSubsystem.xml"/>
</SystemClass>
```

#### AlsaSubsystem.xml
Here we have to create our own parameter:

* Which type is an *Integer*.
* Which *Min*=0 and *Max*=64.

We also need to specify the *Card* name here. As a reminder, our card is named `PCH`.

```xml
<?xml version="1.0" encoding="UTF-8"?>
<Subsystem xmlns:xsi="http://www.w3.org/2001/XMLSchema-instance"
    xsi:noNamespaceSchemaLocation="../../Schemas/Subsystem.xsd"
    Name="alsa" Type="ALSA" Endianness="Little">
    <ComponentLibrary>
        <ComponentType Name="VolumeMixer">
            <!-- Our volume parameter is linked with an alsa control called 'Master Playback Volume' -->
            <!-- Retrieve the parameter information with an cget command on your control -->
            <IntegerParameter Name="volume" Size="8" Min="0" Max="64"
                              Mapping="Control:'Master Playback Volume'"/>
        </ComponentType>
    </ComponentLibrary>
    <InstanceDefinition>
        <!-- Our mixer is on the 'PCH' card, the name can be found in /proc/asound/cards -->
        <Component Name="master" Type="VolumeMixer" Mapping="Card:PCH"/>
    </InstanceDefinition>
</Subsystem>
```

Theses samples are also available on the [samples GitHub](https://github.com/01org/parameter-framework-samples)

Copy the files one next to the other in your current directory.


### Add a bash alias to talk to the parameter-framework
For readability purposes, we use the following alias:

```bash
alias pfw='remote-process localhost 5000'
```


### Tell your system where to link
If you encounter link errors, you should tell your shell where the libraries
required for `remote-process` and `test-platform` can be found:

```bash
# The default installation path is in /usr/local/lib
export LD_LIBRARY_PATH=/usr/local/lib/
```


### Running the example

#### Step 1: Run the test-platform
First of all we must simulate a platform.
In order to do that, we can use the `test-platform` executable from
the parameter-framework.

    test-platform ./ParameterFrameworkConfiguration.xml 5426

It should not provide any output. As specified, it is waiting on *port 5426*
for commands.

#### Step 2: Start the test-platform
To tell the `test-platform` to *start the parameter-framework*, we can use
`remote-process`, which is also available after building the parameter-framework.

    remote-process localhost 5426 start

This should provide a result similar to this one:

    Loading {
        Loading framework configuration {
            Tuning allowed
        } Loading framework configuration
        Loading subsystem plugins {
            Attempting to load subsystem plugin path "/home/myself/dev/pfw/lib/libalsa-subsystem.so"
            All subsystem plugins successfully loaded
        } Loading subsystem plugins
        Loading Audio system class structure
        Importing system structure from file ./AudioStructure.xml {
            Loading ./Alsa.xml {
            } Loading ./Alsa.xml
        } Importing system structure from file ./AudioStructure.xml
        Loading settings {
        } Loading settings
        Main blackboard back synchronization {
        } Main blackboard back synchronization
        Criterion states {
        } Criterion states
        Applying configurations {
        } Applying configurations
        Handling remote processing interface {
            Loading remote processor library
            Starting remote processor server on port 5000
        } Handling remote processing interface
    } Loading

#### Step 3: List the available parameters
Now that the parameter-framework is up and running, we can *list the
available parameters*.

    pfw listParameters /

Which should answer us with `/Audio/alsa/master/volume`.

#### Step 4: Allow the user to tune the values
To be able to change the `volume` parameter, we must tell the parameter-framework
to *turn the tuning mode on*.

    pfw setTuningMode on

#### Step 5: Change the volume via the parameter-framework
Finally, we can change the volume with by setting the `volume` parameter:

    pfw setParameter /Audio/alsa/master/volume 64

Which should reply us by a `Done`

Since we set the volume to *64*, which is the maximum allowed value, we can observe the
change in the `AlsaMixer` utility:

![AlsaMixer result](http://01org.github.io/parameter-framework/hosting/alsamixer_100volume.png "AlsaMixer result")
