// application constants
// Brokers - https://shiftr.io/try
// mqtt://broker.hivemq.com
const mqttBroker = 'https://shiftr.io/try'
const publishTopic = 'hasaccess'
const subscribeTopic1 = 'userdata1'
const subscribeTopic2 = 'userdata2'

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
    const readerCode = rfidDetail[0] && rfidDetail[0].split('reader')[1]

    if (user) {
        console.log('User:', user)
        if (user.reader === rfidDetail[0] && user.hasAccess) {
            return {
                reader: readerCode,
                hasAccess: 'yes'
            }

        }
    } else {
        console.log('Could not identify card. Card not registered.')
    }

    return {
        reader: readerCode,
        hasAccess: 'no'
    }
}

/** 
 * @method sendAccessInformation
 * @description publishes the access string to publishTopic
 * @param access: String
*/
const sendAccessInformation = (access) => {
    client.publish(`${publishTopic}${access.reader}`, access.hasAccess)
    console.log(`Successfully published ${access.hasAccess} to '${publishTopic}${access.reader}'`)
}

client.on('connect', function (message) {
    client.subscribe(subscribeTopic1, function (err) {
        if (!err) {
          console.log(`Successfully subscribed to topic : ${subscribeTopic1}`)
        }
    })
})

client.on('connect', function (message) {
    client.subscribe(subscribeTopic2, function (err) {
        if (!err) {
          console.log(`Successfully subscribed to topic : ${subscribeTopic2}`)
        }
    })
})

 
client.on('message', function (topic, message) {
  // message is Buffer
  const access = checkIfUserHasAccess(message.toString())
  sendAccessInformation(access)
})