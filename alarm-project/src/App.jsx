import { useState, useEffect } from 'react';

function App() {
  const [messages, setMessages] = useState([]);

  useEffect(() => {
    const botToken = '7119578524:AAF7sJPmjsbjJcS3iI-8Ei6-JExDG3BtfCA';
    const url = `https://api.telegram.org/bot${botToken}/getUpdates`;

    // Botunuzdan mesajları almak için bir HTTP GET isteği yapın
    fetch(url)
      .then(response => response.json())
      .then(data => {
        // Gelen mesajları React state'ine kaydedin
        setMessages(data.result);
      })
      .catch(error => {
        console.error('Mesaj alma hatası:', error);
      });
  }, []); // Sadece ilk render zamanında çalışacak şekilde boş bağımlılık dizisi kullanıyoruz

  return (
    <div>
      <h1>Telegram Bot Mesajları</h1>
      <ul>
        {messages.map((message, index) => (
          <li key={index}>{message.message.text}</li>
        ))}
      </ul>
    </div>
  );
}

export default App;
