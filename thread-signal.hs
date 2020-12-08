module Main where

import Control.Concurrent
import Control.Monad
import Foreign.C.Types (CInt(..))
import System.Directory (listDirectory)
import System.Posix

main :: IO ()
main = do
    setHandler sigUSR1 $ Catch $ c_keep_capabilities
    sendSignalToAllNativeThreads
    getUserEntryForName "nobody" >>= setUserID . userID
    setHandler sigUSR1 $ Catch $ c_drop_except_bind
    sendSignalToAllNativeThreads
    threadDelay 10000000

setHandler :: Signal -> Handler -> IO ()
setHandler sig func = void $ installHandler sig func Nothing

sendSignalToAllNativeThreads :: IO ()
sendSignalToAllNativeThreads = do
    pid <- getProcessID
    strtids <- listDirectory ("/proc/" ++ show pid ++ "/task")
    let tids = map read strtids :: [Int]
    mapM_ sendSig tids

sendSig :: Int -> IO ()
sendSig tid = do
    c_send_signal (fromIntegral tid) sigUSR1
    threadDelay 10000

foreign import ccall unsafe "keep_capabilities"
  c_keep_capabilities :: IO ()

foreign import ccall unsafe "drop_except_bind"
  c_drop_except_bind :: IO ()

foreign import ccall unsafe "send_signal"
  c_send_signal :: CInt -> CInt -> IO ()

