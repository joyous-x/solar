

# decorator for async functions
def cache_handler(ttl=0, **kwargs):
    cache_kwargs = kwargs
    def decorator(func):
        async def wrapper(*args, **kwargs):
            # TODO
            result = await func(*args, **kwargs)
            return result
        return wrapper
    return decorator
