// application constants
const mqttBroker = 'mqtt://broker.hivemq.com'
const publishTopic = 'hasaccess'
const subscribeTopic = 'userdata'

// imports and initialization
const mqtt = require('mqtt')
const client = mqtt.connect(mqttBroker);

// User details: This acts like a local DB where user is registered against
// an RFID number and reader
const userData = [{
    rfid: 911686018,
    userId: '1234',
    name: 'user A',
    reader: 'reader1',
    hasAccess: true
}, {
    rfid: 1313622727,
    userId: '1235',
    name: 'user B',
    reader: 'reader1',
    hasAccess: true
}, {
    rfid: 2331774390,
    userId: '1236',
    name: 'user C',
    reader: 'reader2',
    hasAccess: false
}, {
    rfid: 252366689,
    userId: '1237',
    name: 'user D',
    reader: 'reader2',
    hasAccess: true
}]

/** 
 * @method checkIfUserHasAccess
 * @description accepts RFID number along with the reader name seperated by '-'.
 * i.e. ReaderID-RFIDnumber
 * @param userRfid: String
 * @return 'yes if the user has access.
 * @return 'no' if user has no access or user does not exist
*/
const checkIfUserHasAccess = (userRfid) => {
    const rfidDetail = userRfid.split('-')
    const user = userData.filter(({rfid}) => (rfid == rfidDetail[1]))[0]

    if (user) {
        console.log('User:', user)
        if (user.reader === rfidDetail[0] && user.hasAccess) {
            return 'yes'
        }
    } else {
        console.log('Could not identify card. Card not registered.')
    }

    return 'no'
}

/** 
 * @method sendAccessInformation
 * @description publishes the access string to publishTopic
 * @param access: String
*/
const sendAccessInformation = (access) => {
    client.publish(publishTopic, access)
    console.log(`Successfully published '${access}' to ${publishTopic}`)
}

client.on('connect', function (message) {
    client.subscribe(subscribeTopic, function (err) {
        if (!err) {
          console.log(`Successfully subscribed to topic : ${subscribeTopic}`)
        }
    })
})

 
client.on('message', function (topic, message) {
  // message is Buffer
  const access = checkIfUserHasAccess(message.toString())
  sendAccessInformation(access)
})