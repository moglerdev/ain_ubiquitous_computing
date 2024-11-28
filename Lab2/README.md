# Labor 02

## Exercise 1: Hello World

The first assignment was to learn the basics of Node-RED. It was a simple task involving an inject node and a debug node. The inject node sends a message object with the payload "Hello World" to the debug node. With this toolset, it is possible to debug the flow and, for example, inspect the payload of the message after specific nodes.

## Exercise 2: Change World to Mars

In this chapter, the task was to replace the word "World" with "Mars," which requires the use of the change node. The change node can access the `msg`, `flow`, or `global` objects and modify their properties based on their names, even at deeper levels using dot notation. It supports four methods: **change**, **set**, **delete**, and **move**. 

- **Change**: Searches for a specific value (e.g., a string) in the payload and replaces it with another value, such as replacing "World" with "Mars" in this task. It can also work with other data types.  
- **Set**: Assigns a value (e.g., an object or string) directly to a specified property.  
- **Delete**: Removes the specified property.  
- **Move**: Transfers a property to another object, such as moving a `msg` property to the `flow` or `global` object.

To test the change node, the final node in the flow is a debug node.

## Exercise 3: Work with CSV Data from the Web

The USGS website provides access to the latest significant earthquakes of the current month. To fetch this data, the request node can be used with a specific URL. The request node supports various HTTP methods and custom methods if needed.  

Key configurations include:  
- **URL**: Specifies the endpoint to send the request to.  
- **Payload**: Determines how the payload from the previous node is used: ignored (default), appended as URL search parameters (e.g., `?foo=bar&moo=coo`), or sent as the HTTP body.  
- **Secure Connection**: Enabling SSL/TLS adds a field to adjust TLS settings, e.g., for self-signed certificates.  
- **Authentication**: Supports `basic`, `digest`, and `bearer` methods for restricted APIs.  
- **Keep-Alive**: Keeps the connection open to improve latency.  
- **Custom Proxy**: Allows using a proxy server.  
- **Non-OK Responses**: These are sent only to a catch node to avoid undefined behaviors.  
- **Strict HTTP Parsing**: Disabling this helps with non-standard HTTP responses, typically from older servers, but may introduce issues.  
- **Return Type**: Specifies the response format, e.g., string (UTF-8), binary buffer (image), or JSON.  
- **Headers**: Allows adding custom headers for the request.  

The USGS server responds with data in CSV format. To process it, a CSV parser node is used, converting the string into an object in the `msg.payload`. Customization options include:  
- **Separator**: Default is a comma, but it can be set to other delimiters (e.g., semicolon).  
- **Header Row**: Checking `first row contains column names` ensures proper parsing.  
- **Output Mode**: Can send each row as a separate message or the entire data as an array. For this task, each row is sent as an individual message.  

In the next step, the parsed data is sent to a debug node for logging and to a switch node. The switch node checks whether the `mag` property in the payload is greater than or equal to 6. If true, a change node updates the payload to "panic," which is then printed using another debug node.

This flow is triggered every 5 minutes by an inject node.

## Exercise 4: Create a Clock with a Dashboard

Node-RED does not include a dashboard feature by default. To enable this, the `@flowfuse/node-red-dashboard` package is required, which introduces new nodes for displaying data on a dashboard. In this task, the goal was to display the current date and time. Using the `node-red-contrib-moment` package simplifies working with dates and times by adding specialized nodes. Note that installing packages may automatically include required dependencies.

The flow starts with an inject node configured to fire every second and then splits into four lanes:  

1. **Epoch Logging**: The inject node’s payload, the epoch (time in milliseconds since 01/01/1970 00:00), is logged to the console.  

2. **Epoch to Dashboard**:  
   - A text node is used to display the epoch on the dashboard.  
   - For this, the dashboard layout must include a configured page with a group (set up via the FlowFuse package).  
   - The text node is configured with a label, group, and a binding to the payload.  

3. **Formatted Date/Time with Filtering**:  
   - The epoch is formatted to `DD/MM/YY HH:mm` (European standard with a 24-hour clock) using the **Date/Time Formatter** node from the Moment package.  
   - The formatted payload is logged to the console and then sent to a filter node.  
   - The filter ensures the flow continues only when the formatted value changes (every minute), blocking unnecessary updates caused by the inject node firing every second.  
   - After filtering, the payload is displayed on the dashboard via a text node.  

4. **Detailed Date/Time**:  
   - The epoch is formatted to `dddd, MMMM Do YYYY, h:mm:ss a` (e.g., "Monday, September 25th 2024, 5:42:17 pm").  
   - This includes the day of the week, full month name, day, year, and time in 12-hour format with `am/pm`.  
   - Since the formatted value includes seconds, no filter node is needed, as it updates every second.  
   - The formatted payload is logged to the console and displayed on the dashboard via another text node.  

This setup effectively prints both raw and formatted timestamps to the console and dashboard, showcasing different levels of detail.

## Exercise 5: Print Weather information to a Dashboard

Get weather information from an API and also print it to the dashboard, was the last assignment. To get the weather data, the API from `weatherapi.com` was used. Because the Flows are saved in a repository on the internet, the API key is for that not directly set into the flow, is saved by environment variable on the machine. To get the API key, the change node set the property `key`, in the payload, with the environment value `$RED_WEATHER_API_KEY`. The payload is sent to the request node, that appends the payload to the URL, so the API key is always secure. The request node sends then the request to `https://api.weatherapi.com/v1/current.json?q=Konstanz` to receive the current weather in Konstanz in JSON format. After that, the payload is parsed from a JSON string to a normal object. Then the object is sent to three lanes, in the first lane the current temperature should be display as a gauge, in the dashboard. That the gauge node can work with the payload, the payload gets set to the property `msg.payload.current.temp_c` with the change node. To be handover to the gauge node, that is from -20 to +40 degrees Celsius configured and has 10 different colors. In the next lane, the weather should be shown in text form, for this the template node is values can interpret `mustache` code and send it to the text node. Also, it wanted was two charts, one with humidity and temperature. To work with the charts the payload has to be parsed, to parse it correctly the function node was used, with that it is possible to execute custom javascript code. 

```js
const value = msg.payload.current;
return {
    payload: {
        time: new Date(),
        humidity: value.humidity,
        temp: value.temp_c
    }
}
```
The code takes the current weather object, that was received previous from the api. And returns an object with a payload that holds the current time from the execution and the humidity and temp in celsius from the object. The object is delivered to both chart node. The chart node are configured to represent the data as a line and interpolate it linear. Also the new message / data should be appended to the chart, to create a smooth course. The X Axis format is 24h-Format and Timescale Typ and the key (name of property) that is should use is `time` and for y it should use `humidity` for humidity and `temp` for temperature.
