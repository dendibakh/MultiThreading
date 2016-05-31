#pragma once

#include <atomic>

class Mutex
{
public:

  Mutex() : m_lock( false ) {}

  ~Mutex() {}

  void lock( void )
  {
    while(m_lock.exchange( true ) )
      std::this_thread::yield();
  }

  void unlock( void )
  {
    if( !(m_lock.exchange( false )) )
      throw( std::runtime_error( "Someone has stollen our lock." ) );
  }

private:
  std::atomic_bool m_lock;
};

    //std::thread::id mainThreadID; // this thread id will be used to indicate that noone owns a lock right now.
    /*
    class Mutex
    {
    public:
    	void lock()
    	{
    		while ( !ID.compare_exchange_strong(mainThreadID, std::this_thread::get_id()) ) 
    		{}
    		if (ID.load() == mainThreadID)
			throw std::runtime_error("Lock was not obtained");
    	}

    	void unlock()
    	{
    		std::thread::id expected = std::this_thread::get_id();
    		if ( !ID.compare_exchange_strong(expected, mainThreadID) )
    			throw std::runtime_error("Someone has stollen my lock");
    	}
    	Mutex() : ID(mainThreadID) {}
    private:
    	std::atomic<std::thread::id> ID;
    };
    */
