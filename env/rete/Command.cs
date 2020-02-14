using System;

namespace rete
{
    public abstract class Command
    {
        public abstract string Word { get; }
        
        public void AddCommand<T>() where T : Command, new()
        {
            Console.WriteLine("adding " + (new T()).Word);
        }
    }
}