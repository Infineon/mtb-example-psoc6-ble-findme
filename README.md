# PSoC 6 MCU with Bluetooth Low Energy (BLE) Connectivity - Find Me

## Overview

This code example demonstrates the implementation of a simple BLE Immediate Alert Service (IAS)-based Find Me Profile (FMP) using PSoC® 6 MCU with Bluetooth Low Energy (BLE) Connectivity (PSoC 6 BLE). 

This design implements a BLE [FMP](https://www.bluetooth.org/docman/handlers/downloaddoc.ashx?doc_id=239389) that consists of an IAS. FMP and [IAS](https://developer.bluetooth.org/gatt/services/Pages/ServiceViewer.aspx?u=org.bluetooth.service.immediate_alert.xml) are BLE standard Profile and Service respectively, as defined by the Bluetooth SIG. 

The design uses the LEDs on the Kit. USER_LED1 indicates the BLE advertisement (LED blinking) and connection state (ON for connected, OFF for disconnected). USER_LED2 displays the alert level (OFF, flashing, and ON for no alert, mild alert, and high alert respectively).

The USB-BLE dongle provided with the CY8CKIT-062-BLE Pioneer kit or an iOS/Android mobile device can act as the BLE Central device, which locates the Peripheral device.

## Requirements

- [ModusToolbox™ IDE](https://www.cypress.com/products/-modustoolbox-software-environment) v2.0  
- Programming Language: C  
- Associated Parts: All [PSoC® 6 MCU](http://www.cypress.com/PSoC6) parts with BLE connectivity  

## Supported Kits

- [CY8CKIT-062-BLE PSoC 6 BLE Pioneer Kit](https://www.cypress.com/CY8CKIT-062-BLE) (CY8CKIT-062-BLE) - Default target 
- [CY8CPROTO-063-BLE PSoC 6 BLE Prototyping Kit](http://www.cypress.com/cy8cproto-063-ble) (CY8CPROTO-063-BLE) 
  
  
## Hardware Setup

This example uses the kit’s default configuration. Refer to the kit guide to ensure the kit is configured correctly.

**Note**: The PSoC 6 BLE Pioneer kit ships with KitProg2 installed. ModusToolbox software requires KitProg3. Before using this code example, make sure that the kit is upgraded to KitProg3. The tool and instructions are available in the [Firmware Loader](https://github.com/cypresssemiconductorco/Firmware-loader) GitHub repository. If you do not upgrade, you will see an error like “unable to find CMSIS-DAP device” or “KitProg firmware is out of date”.

## Software Setup

This code example consists of two parts: a locator and a target. 

For the locator, download and install either the [CySmart Host Emulation Tool](http://cypress.com/cysmart) PC application or the CySmart app for iOS or Android. You can test behavior with any of the two options, but the CySmart app is simpler.

Scan the following QR codes from your mobile phone to download the CySmart app.

![qr_code](images/qr_code.png)

## Using the Code Example 

#### In the ModusToolbox IDE

1. Click the **New Application** link in the Quick Panel (or, use **File > New > ModusToolbox IDE Application**).

2. Pick a kit supported by the code example from the list shown in the **IDE Application** dialog. 

   When you select a supported kit, the example is reconfigured automatically to work with the kit. To work with a different supported kit later, use the **Library Manager** to choose the BSP for the supported kit. You can use the Library Manager to select or update the BSP and firmware libraries used in this application. To access the Library Manager, right-click the application name from the Project Workspace window in the IDE, and select **ModusToolbox > Library Manager**. For more details, see the IDE User Guide: *{ModusToolbox install directory}/ide_2.0/docs/mt_ide_user_guide.pdf*.

   You can also just start the application creation process again and select a different kit. 

   If you want to use the application for a kit not listed here, you may need to update the source files. If the kit does not have the required resources, the application may not work.

3. In the **Starter Application** window, choose the example.

4. Click **Next** and complete the application creation process.

See [Importing Code Example into ModusToolbox IDE - KBA225201](https://community.cypress.com/docs/DOC-15968) for details.

#### In Command Line Tools

1. Download and unzip this repository onto your local machine, or clone the repository.
2. Open the CLI terminal and navigate to the application folder.
3. Import required libraries by executing the command 
      ```
      make getlibs
      ```

## Operation 

1. Connect the board to your PC using the provided USB cable.
2. Open a terminal program and select the KitProg3 COM port. Set the serial port parameters to 8N1 and 115200 baud.
3. Program the board.

   #### Using ModusToolbox IDE

   1. Select the application project in the Project Explorer.
   2. In the Quick Panel, scroll down, and click **\<Application Name> Program (KitProg3)**.

   #### Using CLI

   1. From the CLI terminal, execute the following command to build and program for the default target (CY8CKIT-062-BLE).

      ```
      make program
      ```

      You can also specify the target and tool chain.  
      ```
      make program TARGET=<BSP> TOOLCHAIN=<toolchain>
      ``` 
      
      The following command shows an example for CY8CPROTO-063-BLE and GCC_ARM toolchain:

      ```
      make program TARGET=CY8CPROTO-063-BLE TOOLCHAIN=GCC_ARM
      ```

4. After programming, USER_LED1 starts blinking indicating that BLE is advertising, and prints a message on the terminal application as shown in [Figure 1](#Figure-1-Terminal-Output).

   ##### Figure 1. Terminal Output
   ![Terminal Output](images/fig1.png)

5. To test using the CySmart mobile app:
   1. Turn ON Bluetooth on your Android or iOS device.

   2. Launch the CySmart app.

   3. Press the reset switch on the Kit to start BLE advertisements from your design. USER LED1 starts toggling, indicating that BLE advertisement has started.

   4. Pull down the CySmart app home screen to start scanning for BLE Peripherals; your device appears in the CySmart app home screen. Select your device to establish a BLE connection. Once the connection is established, USER_LED1 stays ON.

   5. Select the 'Find Me' Profile from the carousel view.

   6. Select an alert Level value on the Find Me Profile screen. Observe that the state of USER_LED2 on the device changes based on the alert level.

      ##### Figure 2. Testing with the CySmart App on iOS
      ![Testing with the CySmart App on iOS](images/fig2.png)

      ##### Figure 3. Testing with the CySmart App on Android
      ![Testing with the CySmart App on Android](images/fig3.png)

6. To test using the CySmart Host Emulation Tool:

   1. Connect the BLE Dongle to your Windows PC. Wait for the driver installation to complete.

   2. Launch the CySmart Host Emulation Tool.

      **Note:** If the dongle firmware is outdated, you will be alerted. You must upgrade the firmware before you can complete this step.
      Follow the instructions in the window to update the dongle firmware.

   3. Select **Configure Master Settings** and then click **Restore Defaults**, as shown in [Figure 4](#Figure-4-CySmart-Master-Settings-Configuration). Then, click **OK**.

      ##### Figure 4. CySmart Master Settings Configuration
      ![cysmart1](images/fig4.png)

   4. Press the reset switch on the Pioneer Kit to start BLE advertisements from your design.

   5. On the CySmart Host Emulation Tool, click **Start Scan**. Your device name (configured as Find Me Target) should appear in the **Discovered devices** list, as shown in [Figure 5](#Figure-5-CySmart-Device-Discovery).

      ##### Figure 5. CySmart Device Discovery
      ![CySmart Device Discovery](images/fig5.png)

   6. Select your device and click **Connect** to establish a BLE connection between the CySmart Host Emulation Tool and your device, as shown in [Figure 6](#Figure-6-CySmart-Device-Connection).

      ##### Figure 6. CySmart Device Connection
      ![CySmart Device Connection](images/fig6.png)

   7. Once connected, switch to the Find Me Target device tab and discover all Attributes on your design from the CySmart Host Emulation Tool, as shown in [Figure 7](#Figure-7-CySmart-Attribute-Discovery).

      ##### Figure 7. CySmart Attribute Discovery
      ![CySmart Attribute Discovery](images/fig7.png)

   8. Scroll down the Attributes window and locate the Immediate Alert Service fields. Write a value of 0 – no alert, 1 – mild alert, or 2 – high alert to the Alert Level Characteristic under the Immediate Alert Service, as [Figure 8](#figure-8-testing-with-cysmart-host-emulation-tool) shows. Observe that the state of the LED on your device changes per your Alert Level Characteristic configuration.

      ##### Figure 8. Testing with CySmart Host Emulation Tool
      ![Testing with CySmart Host Emulation Tool](images/fig8.png)

## Debugging

You can debug the example to step through the code. In the ModusToolbox IDE, use the **\<Application Name> Debug (KitProg3)** configuration in the **Quick Panel**. See [Debugging a PSoC 6 MCU ModusToolbox Project - KBA224621](https://community.cypress.com/docs/DOC-15763) for details.

## Design and Implementation

The ‘Find Me Locator’ (the BLE Central device) is a BLE GATT Client. The ‘Find Me Target’ (the Peripheral device) is a BLE GATT Server with the IAS and an additional Device Information Service implemented, as [Figure 9](#figure-9-find-me-servicerelationship) shows.

##### Figure 9. Find Me Service Relationship
![Find Me Service Relationship](images/fig9.png)  

The BLE Find Me profile defines what happens when the locating Central device broadcasts a change in the alert level. The Find Me locator performs service discovery using the 'GATT Discover All Primary Services' procedure. The BLE Service Characteristic discovery is done by the 'Discover All Characteristics of a Service' procedure. When the Find Me Locator wants to cause an alert on the Find Me Target, it writes an alert level in the Alert Level Characteristic of the IAS. When the Find Me Target receives an alert level, it indicates the level using the user LED2: OFF for no alert, blinking for mild alert, and ON for high alert.  

The BLE interface is implemented on a PSoC 6 MCU with BLE Connectivity device using the BLE resource. The application runs on the Arm® Cortex®-M4 CPU.  

See [AN210781 – Getting Started with PSoC 6 MCU with Bluetooth Low Energy (BLE) Connectivity](http://www.cypress.com/an210781) to understand the design of firmware for this code example. The device enters low-power Deep Sleep mode when BLE is idle. It wakes up automatically when there is activity on the BLE connection.  

When BLE is disconnected, the device enters Hibernate mode. It wakes up when the reset switch or wakeup switch (SW2) is pressed and performs a complete reset sequence in firmware.  

The project uses [Bluetooth Low Energy Middleware](https://github.com/cypresssemiconductorco/bless); see [PSoC 6 BLE Middleware API Reference Guide](https://cypresssemiconductorco.github.io/bless/ble_api_reference_manual/html/index.html) for more information on APIs. The [Quick Start section](https://cypresssemiconductorco.github.io/bless/ble_api_reference_manual/html/page_ble_quick_start.html) of the PSoC 6 BLE Middleware API Reference Guide describes step-by-step instructions to configure and launch PSoC 6 BLE Middleware.  

## Related Resources

| Application Notes                                            |                                                              |
| :----------------------------------------------------------- | :----------------------------------------------------------- |
| [AN221774](https://www.cypress.com/AN221774) – Getting Started with PSoC 6 MCU on PSoC Creator | Describes PSoC 6 MCU devices and how to build your first application with PSoC Creator |
| [AN228571](https://www.cypress.com/AN228571) – Getting Started with PSoC 6 MCU on ModusToolbox | Describes PSoC 6 MCU devices and how to build your first application with ModusToolbox |
| [AN210781](https://www.cypress.com/AN210781) – Getting Started with PSoC 6 MCU with Bluetooth Low Energy (BLE) Connectivity on PSoC Creator | Describes PSoC 6 MCU with BLE Connectivity devices and how to build your first application with PSoC Creator |
| [AN215656](https://www.cypress.com/AN215656) – PSoC 6 MCU: Dual-CPU System Design | Describes the dual-CPU architecture in PSoC 6 MCU, and shows how to build a simple dual-CPU design |
| **Code Examples**                                            |                                                              |
| [Using ModusToolbox IDE](https://github.com/cypresssemiconductorco/Code-Examples-for-ModusToolbox-Software) | [Using PSoC Creator](https://www.cypress.com/documentation/code-examples/psoc-6-mcu-code-examples) |
| **Device Documentation**                                     |                                                              |
| [PSoC 6 MCU Datasheets](https://www.cypress.com/search/all?f[0]=meta_type%3Atechnical_documents&f[1]=resource_meta_type%3A575&f[2]=field_related_products%3A114026) | [PSoC 6 Technical Reference Manuals](https://www.cypress.com/search/all/PSoC%206%20Technical%20Reference%20Manual?f[0]=meta_type%3Atechnical_documents&f[1]=resource_meta_type%3A583) |
| **Development Kits**                                         | Available at Cypress.com                                     |
| [CY8CKIT-062-BLE](https://www.cypress.com/CY8CKIT-062-BLE) PSoC 6 BLE Pioneer Kit | [CY8CKIT-062-WiFi-BT](https://www.cypress.com/CY8CKIT-062-WiFi-BT) PSoC 6 WiFi-BT Pioneer Kit |
| [CY8CPROTO-063-BLE](https://www.cypress.com/CY8CPROTO-063-BLE) PSoC 6 BLE Prototyping Kit | [CY8CPROTO-062-4343W](https://www.cypress.com/cy8cproto-062-4343w) PSoC 6 Wi-Fi BT Prototyping Kit |
| **Middleware**                                               | Middleware libraries are distributed on GitHub               |
| PSoC 6 Peripheral Driver Library and docs                    | [psoc6pdl](https://github.com/cypresssemiconductorco/psoc6pdl) on GitHub |
| Bluetooth Low Energy Middleware                           | [bless](https://github.com/cypresssemiconductorco/bless) on GitHub |
| CapSense library and docs                                    | [capsense](https://github.com/cypresssemiconductorco/capsense) on GitHub |
| Links to all PSoC 6 Middleware                               | [psoc6-middleware](https://github.com/cypresssemiconductorco/psoc6-middleware) on GitHub |
| **Tools**                                                    |                                                              |
| [ModusToolbox IDE](https://www.cypress.com/modustoolbox)     | The Cypress IDE for PSoC 6 and IoT designers                 |
| [PSoC Creator](https://www.cypress.com/products/psoc-creator-integrated-design-environment-ide) | The Cypress IDE for PSoC and FM0+ development                | 

## Other Resources

Cypress provides a wealth of data at www.cypress.com to help you to select the right device, and quickly and effectively integrate the device into your design.

For PSoC 6 MCU devices, see [KBA223067](https://community.cypress.com/docs/DOC-14644) in the Cypress community for a comprehensive list of PSoC 6 MCU resources.

## Document History

Document Title: **CE212736 - PSoC 6 MCU with Bluetooth Low Energy (BLE) Connectivity - Find Me**

| Revision |Description of Change |
| -------- |--------------------- |
| 1.0.0    | New code example     |

All other trademarks or registered trademarks referenced herein are the property of their respective
owners.

![Banner](images/Banner.png)

-------------------------------------------------------------------------------

© Cypress Semiconductor Corporation, 2019. This document is the property of Cypress Semiconductor Corporation and its subsidiaries (“Cypress”).  This document, including any software or firmware included or referenced in this document (“Software”), is owned by Cypress under the intellectual property laws and treaties of the United States and other countries worldwide.  Cypress reserves all rights under such laws and treaties and does not, except as specifically stated in this paragraph, grant any license under its patents, copyrights, trademarks, or other intellectual property rights.  If the Software is not accompanied by a license agreement and you do not otherwise have a written agreement with Cypress governing the use of the Software, then Cypress hereby grants you a personal, non-exclusive, nontransferable license (without the right to sublicense) (1) under its copyright rights in the Software (a) for Software provided in source code form, to modify and reproduce the Software solely for use with Cypress hardware products, only internally within your organization, and (b) to distribute the Software in binary code form externally to end users (either directly or indirectly through resellers and distributors), solely for use on Cypress hardware product units, and (2) under those claims of Cypress’s patents that are infringed by the Software (as provided by Cypress, unmodified) to make, use, distribute, and import the Software solely for use with Cypress hardware products.  Any other use, reproduction, modification, translation, or compilation of the Software is prohibited.
TO THE EXTENT PERMITTED BY APPLICABLE LAW, CYPRESS MAKES NO WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, WITH REGARD TO THIS DOCUMENT OR ANY SOFTWARE OR ACCOMPANYING HARDWARE, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE.  No computing device can be absolutely secure.  Therefore, despite security measures implemented in Cypress hardware or software products, Cypress shall have no liability arising out of any security breach, such as unauthorized access to or use of a Cypress product.  CYPRESS DOES NOT REPRESENT, WARRANT, OR GUARANTEE THAT CYPRESS PRODUCTS, OR SYSTEMS CREATED USING CYPRESS PRODUCTS, WILL BE FREE FROM CORRUPTION, ATTACK, VIRUSES, INTERFERENCE, HACKING, DATA LOSS OR THEFT, OR OTHER SECURITY INTRUSION (collectively, “Security Breach”).  Cypress disclaims any liability relating to any Security Breach, and you shall and hereby do release Cypress from any claim, damage, or other liability arising from any Security Breach.  In addition, the products described in these materials may contain design defects or errors known as errata which may cause the product to deviate from published specifications.  To the extent permitted by applicable law, Cypress reserves the right to make changes to this document without further notice. Cypress does not assume any liability arising out of the application or use of any product or circuit described in this document.  Any information provided in this document, including any sample design information or programming code, is provided only for reference purposes.  It is the responsibility of the user of this document to properly design, program, and test the functionality and safety of any application made of this information and any resulting product.  “High-Risk Device” means any device or system whose failure could cause personal injury, death, or property damage.  Examples of High-Risk Devices are weapons, nuclear installations, surgical implants, and other medical devices.  “Critical Component” means any component of a High-Risk Device whose failure to perform can be reasonably expected to cause, directly or indirectly, the failure of the High-Risk Device, or to affect its safety or effectiveness.  Cypress is not liable, in whole or in part, and you shall and hereby do release Cypress from any claim, damage, or other liability arising from any use of a Cypress product as a Critical Component in a High-Risk Device.  You shall indemnify and hold Cypress, its directors, officers, employees, agents, affiliates, distributors, and assigns harmless from and against all claims, costs, damages, and expenses, arising out of any claim, including claims for product liability, personal injury or death, or property damage arising from any use of a Cypress product as a Critical Component in a High-Risk Device.  Cypress products are not intended or authorized for use as a Critical Component in any High-Risk Device except to the limited extent that (i) Cypress’s published data sheet for the product explicitly states Cypress has qualified the product for use in a specific High-Risk Device, or (ii) Cypress has given you advance written authorization to use the product as a Critical Component in the specific High-Risk Device and you have signed a separate indemnification agreement.
Cypress, the Cypress logo, Spansion, the Spansion logo, and combinations thereof, WICED, PSoC, CapSense, EZ-USB, F-RAM, and Traveo are trademarks or registered trademarks of Cypress in the United States and other countries.  For a more complete list of Cypress trademarks, visit cypress.com.  Other names and brands may be claimed as property of their respective owners.