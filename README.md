This repository features the firmware codes associated with the design of a family of devices created for

## Platorm

* Hardware: Custom printed circuit board featuring the Texas Instruments CC1312R1 System on Chip.
  * Radio Communication: 150 MHz band, 2-GFSK modulation. The radio is included inside the CC1312R1 chip.
  * Sensors:  
    * IMU: Temperature + Accelerometer + Gyroscope + Magnetometer.
    * GPS receptor U-blox NEO 7M: Position.
  * Lights: 3 leds
  * Battery: LiPo 3.7V-600mAh.
  * Battery Charger: USB-B micro. 
  * SD Slot: Allow the acquired information from sensors to be stored in the SD CARD.
* Firmware and SDK:
  * IDE: Code Composer Studio Version 11.1
  * SDK: [Simple Link SDK Version 6.30](https://www.ti.com/tool/download/SIMPLELINK-CC13XX-CC26XX-SDK/6.30.01.03)
  * Compiler: TI Clang Version 2.1
* Software:
  * Decoder: Python. Convert binary information stored in the SD card to human readable information.
  
## Firmware features: Table of Contents

- [Modular Design and Scalability](#modular-design-and-scalability)
- [State Machine and Task Activation Modes](#state-machine-and-task-activation-modes)
- [Power Consumption Optimization](#power-consumption-optimization)
- [Conclusion](#conclusion)

## Modular Design and Scalability

The firmware embraces a modular design, which facilitates maintainability and scalability. Each functional block is encapsulated as a separate module, promoting reusability and easier management of complex systems. As the firmware grows, additional functionalities can be integrated without disrupting the existing codebase. This modularity also enables collaboration among multiple developers, each focusing on specific modules.

The scalable architecture ensures that the firmware accommodates future enhancements and modifications without requiring a complete overhaul. New sensor tasks, communication protocols, or data processing methods can be integrated smoothly, making the system adaptable to evolving requirements.

## State Machine and Task Activation Modes

The firmware employs a state machine model to manage the device's behavior. It operates in different modes, each optimized for specific scenarios:

1. **Normal Mode:** This mode includes a set of tasks actively engaged based on the device's intended operation. For instance, during standard data collection, sensor tasks and communication modules are active.

2. **Low-Power Mode:** In situations where continuous operation isn't necessary, the system can transition to a low-power state. Unnecessary tasks are suspended, reducing power consumption while maintaining essential functionalities like periodic activity monitoring.

3. **Animal Behavior-Dependent Mode:** To optimize power usage further, the firmware monitors animal behavior and adjusts its operation accordingly. If the animal remains inactive for a specified duration, non-critical tasks are suspended, and the device enters a sleep state. This feature minimizes energy consumption during periods of inactivity.

## Power Consumption Optimization

The firmware's design, state machine operation, and task activation modes directly impact power consumption:

- **Modular Deactivation:** Inactive modules consume minimal power, contributing to overall energy efficiency. During low-power modes or when specific tasks are not required, the firmware deactivates those modules, drastically reducing power draw.

- **Selective Task Activation:** The ability to activate or deactivate tasks based on the device's current mode is a significant power-saving feature. In Animal Behavior-Dependent Mode, the firmware selectively activates tasks related to animal activity monitoring, effectively saving energy.

- **Dynamic Frequency Scaling:** Certain modules, like the RF module, might consume higher power during transmission. By dynamically adjusting the transmission frequency based on system requirements, the firmware can further optimize energy consumption without compromising essential operations.

- **State Transition Logic:** Transitions between different modes are managed intelligently, ensuring that power-hungry tasks are only active when necessary. This dynamic switching minimizes energy expenditure and extends the device's operational time.

## Conclusion

The firmware's modular design, scalability, state machine operation, and power consumption optimization collectively contribute to a robust and adaptable system. The ability to tailor the device's behavior based on task activation and animal behavior not only ensures efficient energy usage but also enhances the overall reliability and longevity of the device's operation.

For detailed implementation guidelines and real-world use cases, consult the comprehensive documentation within the repository.

# tasks

#### Interaction Between `printuf` and `task_uart0_printu_print` Tasks

In complex software systems, tasks often need to communicate and synchronize their actions to achieve coordinated behavior. Let's explore the interaction between two tasks, `printuf` and `task_uart0_printu_print`, in the context of printing and UART communication.

##### Task Descriptions

- **printuf Function**: The `printuf` function handles formatted printing, acquiring a semaphore to add data to the print queue. It formats the input string, appends it to the queue if space is available, and signals the `task_uart0_printu_print` task to print.

- **task_uart0_printu_print Function**: The `task_uart0_printu_print` task waits for a semaphore indicating data availability in the print queue. When data is present, it extracts and sends the data over UART, ensuring synchronization with UART access.

##### Interaction Overview

These tasks interact through semaphores to ensure orderly data handling and UART communication. Elements shared between the tasks are marked with a consistent turquoise color for easy identification.

Let's visualize this interaction using the following flowcharts:

##### Flowcharts: 

<style>
  .shared-element {
      #40E0D0; /* Turquoise color */
  }
</style>


<!DOCTYPE html>
<html>
<head>
  <script type="module">
    import mermaid from 'https://cdn.jsdelivr.net/npm/mermaid@10/dist/mermaid.esm.min.mjs';
    mermaid.initialize({ startOnLoad: true });
  </script>
</head>
<body>
  <h2>Flowchart for printuf Function</h2>
<div class="mermaid">
  graph TD
    A[Start] -->|Acquire sem_print_queue_add| B[Parse and format input]
    B -->|Append to queue| C[If queue has space] -->|Release sem_print_queue_add| D[If queue is not empty] -->|Release sem_print_wait| E[End]
    
    classDef shared-element fill:#40E0D0;
    class B shared-element;
    class C shared-element;
    class D shared-element;
</div>


  <h2>Flowchart for task_uart0_printu_print Function</h2>
<div class="mermaid">
  graph TD
    A[Start] -->|Wait for sem_print_queue_add| B[If queue has elements]
    B -->|Copy element from queue| C[Acquire psem_uart0]
    C -->|Initialize and send data| D[Close UART and release psem_uart0]
    D -->|Release sem_print_queue_add| E[Loop back]
    
    classDef shared-element fill:#40E0D0;
    class B shared-element;
    class D shared-element;
</div>

</body>
</html>



#### GPS Data Acquisition Task

The provided code snippet represents a GPS data acquisition task designed to run within an RTOS environment. This task is responsible for interfacing with a GPS module, parsing incoming GPS data, and storing the acquired information in memory. The task demonstrates a comprehensive flow that encompasses GPS module interaction, data parsing, storage management, and interaction with other parts of the system.

#### Key Components:

 * GPS Module Interaction: The task initializes and interacts with a GPS module using UART communication. It configures the GPS module to receive specific types of messages and processes the received bytes in a callback function. This interaction ensures continuous reception of GPS data.

 *  GPS Data Parsing: The code employs the TinyGPS++ library to parse the received GPS data. It extracts information such as latitude, longitude, date, time, and precision. The parsed data is stored in appropriate data structures.

 * Data Storage: The task utilizes memory structures to store the parsed GPS data. It maintains buffers to accumulate messages and organizes them into structured blocks for storage. The task controls the flow of data storage, ensuring that required messages are stored correctly.

  * RF Data Transmission: The task passes relevant GPS data to an RF module for transmission. It prepares data packets containing information like latitude, longitude, and time, which can be transmitted wirelessly.

  * State Machine Integration: The task interacts with a state machine to coordinate its operation. It waits for a specific state to be reached before initiating GPS data acquisition. Additionally, the task responds to system low-power states by exiting its processing loop.

  * Periodic Acquisition: After an initial GPS data acquisition, the task enters a periodic acquisition loop. It repeatedly acquires GPS data, parses it, stores it, and updates RF transmission packets. The acquisition interval is determined by the specified frequency.

####  Flow and Interaction:

The task initializes UART communication with the GPS module and configures the module's behavior. It interacts with the GPS module through callback functions triggered by UART reception. The received data is accumulated and processed using the TinyGPS++ library.

The parsed GPS data is then stored in memory structures. These structures are designed to organize and store different types of GPS messages, including information about location, time, and precision. The task also handles the transmission of parsed GPS data to an RF module for wireless communication.

The GPS data acquisition task coordinates with a state machine, ensuring proper synchronization with the system's operational states. Additionally, the task accommodates system low-power states, allowing for efficient resource utilization.

Overall, this GPS data acquisition task demonstrates a well-structured approach to interfacing with GPS modules, parsing data, and managing acquired information within an RTOS environment.

#### GPS task Diagrams
```mermaid
flowchart LR
    subgraph GPS Data Acquisition Task
    InitializeGPS --> ParseGPSData --> StoreData --> PassDataToRFModule --> CheckGPSAcquired
    end

    InitializeGPS -->|Start| ParseGPSData
    ParseGPSData -->|Success| StoreData
    StoreData -->|Continue| PassDataToRFModule
    PassDataToRFModule -->|Continue| CheckGPSAcquired
    CheckGPSAcquired -->|Yes| ParseGPSData
    CheckGPSAcquired -->|No| Stop

    subgraph CheckGPSAcquired
    style CheckGPSAcquired fill:#f9f,stroke:#333,stroke-width:2px
    Yes((Yes))
    No((No))
    end