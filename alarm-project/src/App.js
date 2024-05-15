import './App.css';

import { alarmRef } from './firebase';
import { onValue } from "firebase/database";

let previousMessageElement = null; // Önceki mesajı tutacak değişken

onValue(alarmRef, (snapshot) => {
  const alarmValue = snapshot.val();
  console.log("Alarm: ", alarmValue);

  // Eğer alarmValue.bool true ise "Hırsız geldi" mesajını ve resmini göster
  if (alarmValue && alarmValue.bool === true) {
    // Yeni bir mesaj oluştur
    const messageElement = document.createElement("div");
    messageElement.textContent = "🚨 Hırsız geldi 🚨";
    messageElement.classList.add("blinking");

    // Eğer önceki bir mesaj varsa, onu sil
    if (previousMessageElement && previousMessageElement.parentNode) {
      previousMessageElement.parentNode.removeChild(previousMessageElement);
    }

    // Yeni mesajı ekrana ekle
    document.body.appendChild(messageElement);

    // Önceki mesajı güncelle
    previousMessageElement = messageElement;


  } else if (alarmValue && alarmValue.bool === false) {
    // Eğer alarmValue.bool false ise "Hırsız yok" mesajını göster
    // Yeni bir mesaj oluştur
    const messageElement = document.createElement("div");
    messageElement.textContent = "Şu anda ev güvenli ✅";

    // Eğer önceki bir mesaj varsa, onu sil
    if (previousMessageElement && previousMessageElement.parentNode) {
      previousMessageElement.parentNode.removeChild(previousMessageElement);
    }

    // Yeni mesajı ekrana ekle
    document.body.appendChild(messageElement);

    // Önceki mesajı güncelle
    previousMessageElement = messageElement;
  }
});



function App() {
  return (
    <div className="App">
      <h1>Hırsız Alarmı</h1>
      <img src="/src/alarm-icon.png" alt="" />
    </div>
  );
}

export default App;
