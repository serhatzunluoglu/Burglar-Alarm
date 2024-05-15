// Import the functions you need from the SDKs you need
import { initializeApp } from "firebase/app";
import { getAnalytics } from "firebase/analytics";
import { getDatabase, ref, onValue } from "firebase/database";

// Your web app's Firebase configuration
const firebaseConfig = {
  apiKey: "your-api-key",
  authDomain: "thief-alarm-deneyap.firebaseapp.com",
  databaseURL: "your-database-url",
  projectId: "thief-alarm-deneyap",
  storageBucket: "thief-alarm-deneyap.appspot.com",
  messagingSenderId: "857900483077",
  appId: "1:857900483077:web:857720079546c876cc526c",
  measurementId: "G-5ZZ7G56KMK"
};

// Initialize Firebase
const app = initializeApp(firebaseConfig);
// eslint-disable-next-line no-unused-vars
const analytics = getAnalytics(app);


// Initialize Realtime Database and get a reference to the service
const database = getDatabase(app);

// Reference to your alarm variable
const alarmRef = ref(database, 'alarm');

// Listen for value changes
onValue(alarmRef, (snapshot) => {
  // eslint-disable-next-line no-unused-vars
  const alarmValue = snapshot.val();
});

export { alarmRef };
