import './App.css'
import MainDash from './views/MainDash/MainDash';
import DevicePage from './views/DevicePage/DevicePage';
import { BrowserRouter, Routes, Route } from 'react-router-dom';

function App() {
  return (
    <>
    <BrowserRouter>
      <Routes>
        <Route path="/" element={<MainDash/>} />
        <Route path="/farm" element={<DevicePage/>} />
      </Routes>
    </BrowserRouter>
    </>
  )
}

export default App
